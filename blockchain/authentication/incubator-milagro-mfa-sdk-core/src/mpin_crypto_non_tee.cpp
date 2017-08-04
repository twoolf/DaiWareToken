/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/*
 * Internal M-Pin Crypto interface Non-TEE implementation
 */

#include "mpin_crypto_non_tee.h"


typedef MPinSDK::String String;
typedef MPinSDK::Status Status;

class Octet : public octet
{
public:
    Octet(size_t maxSize);
    Octet(const String& str);
    ~Octet();
    String ToString();

public:
    static const int TOKEN_SIZE = 2 * PFS + 1;
    static const int GROUP_SIZE = PGS;
};

Octet::Octet(size_t maxSize)
{
    this->max = 0;
    this->len = 0;
    this->val = (char *) calloc(maxSize, 1);
	if(this->val != NULL)
	{
		this->max = maxSize;		
	}		
}

Octet::Octet(const String& str)
{
    this->max = 0;
    this->len = 0;
    size_t maxSize = str.size();
    this->val = (char *) malloc(maxSize);
	if(this->val != NULL)
	{
		this->max = maxSize;
		this->len = maxSize;
		memcpy(this->val, str.c_str(), maxSize);		
	}
}

Octet::~Octet()
{
    if(this->val != NULL)
    {
        memset(this->val, 0, this->max);
        free(this->val);
    }
}

String Octet::ToString()
{
    return String(this->val, this->len);
}


MPinCryptoNonTee::MPinCryptoNonTee() : m_storage(NULL), m_initialized(false), m_sessionOpened(false)
{
}

MPinCryptoNonTee::~MPinCryptoNonTee()
{
    Destroy();
}

Status MPinCryptoNonTee::Init(IStorage *storage)
{
    if(m_initialized)
    {
        return Status(Status::OK);
    }

    m_storage = storage;

    String tokensJson;
    if(!storage->GetData(tokensJson))
    {
        return Status(Status::STORAGE_ERROR, String().Format("Failed to load data from storage: '%s'", storage->GetErrorMessage().c_str()));
    }

    m_tokens.Clear();

    tokensJson.Trim();
    if(tokensJson.length() > 0)
    {
        if(!m_tokens.Parse(tokensJson.c_str()))
        {
            return Status(Status::STORAGE_ERROR, String("Failed to parse tokens json"));
        }
    }

    m_initialized = true;

    return Status(Status::OK);
}

void MPinCryptoNonTee::Destroy()
{
    if(m_initialized)
    {
        CloseSession();
        m_initialized = false;
    }
}

Status MPinCryptoNonTee::OpenSession()
{
    if(m_sessionOpened)
    {
        return Status(Status::CRYPTO_ERROR, String("Session is already opened"));
    }

    if(!m_initialized)
    {
        return Status(Status::CRYPTO_ERROR, String("Not initialized"));
    }

    m_sessionOpened = true;
    return Status(Status::OK);
}

void MPinCryptoNonTee::CloseSession()
{
    if(m_sessionOpened)
    {
        ForgetPass2Data();
        m_sessionOpened = false;
    }
}

Status MPinCryptoNonTee::Register(UserPtr user, const String& pin, std::vector<String>& clientSecretShares)
{
    const String& mpinId = user->GetMPinId();
    
    if(!m_sessionOpened)
    {
        return Status(Status::CRYPTO_ERROR, String("Session is not opened or not initialized"));
    }

    // Combine the secret shares
    if(clientSecretShares.size() != 2)
    {
        return Status(Status::CRYPTO_ERROR, String().Format("Expecting 2 client secret shares (provided=%d)", (int) clientSecretShares.size()));
    }

    Octet cs1(clientSecretShares[0]);
    Octet cs2(clientSecretShares[1]);
    Octet token(Octet::TOKEN_SIZE);

    int res = MPIN_RECOMBINE_G1(&cs1, &cs2, &token);
    if(res)
    {
        return Status(Status::CRYPTO_ERROR, String().Format("MPIN_RECOMBINE_G1() failed with code %d", res));
    }

    if(pin.empty())
    {
        return Status(Status::PIN_INPUT_CANCELED, "Pin input canceled");
    }

    // Extract the pin from the secret
    Octet cid(mpinId);
    res = MPIN_EXTRACT_PIN(&cid, pin.GetHash(), &token);
    if(res)
    {
        return Status(Status::CRYPTO_ERROR, String().Format("MPIN_EXTRACT_PIN() failed with code %d", res));
    }

    // Save the token securely
    if(!StoreToken(mpinId, token.ToString()))
    {
        return Status(Status::STORAGE_ERROR, String("Failed to store token"));
    }

    return Status(Status::OK);
}

Status MPinCryptoNonTee::AuthenticatePass1(UserPtr user, const String& pin, int date, std::vector<String>& timePermitShares, String& commitmentU, String& commitmentUT)
{
    const String& mpinId = user->GetMPinId();

    if(!m_sessionOpened)
    {
        return Status(Status::CRYPTO_ERROR, String("Session is not opened or not initialized"));
    }

    Octet timePermit(Octet::TOKEN_SIZE);

    // Valid date (date != 0) means authentication *with* time permit and 2 time permit shares are expected
    // Invalid date (date == 0) means authentication with *no* time permit and time permit shares are ignored
    if(date != 0)
    {
        // Combine time permit shares
        if(timePermitShares.size() != 2)
        {
            return Status(Status::CRYPTO_ERROR, String().Format("Expecting 2 time permit shares (provided=%d)", (int) timePermitShares.size()));
        }

        Octet tp1(timePermitShares[0]);
        Octet tp2(timePermitShares[1]);

        int res = MPIN_RECOMBINE_G1(&tp1, &tp2, &timePermit);
        if(res)
        {
            return Status(Status::CRYPTO_ERROR, String().Format("MPIN_RECOMBINE_G1() failed with code %d", res));
        }
    }

    // Get the stored token
    String tokenStr = GetToken(mpinId);
    if(tokenStr.length() == 0)
    {
        return Status(Status::CRYPTO_ERROR, String().Format("Failed to find stored token for mpinId='%s'", mpinId.c_str()));
    }

    if(pin.empty())
    {
        return Status(Status::PIN_INPUT_CANCELED, "Pin input canceled");
    }

    // Gather the parameters for Authentication pass 1

    Octet cid(mpinId);

    Octet seed(100);
    seed.len = seed.max;
    GenerateRandomSeed(seed.val, seed.len);
    csprng rng;
    CREATE_CSPRNG(&rng,&seed);

    Octet x(Octet::GROUP_SIZE);
    Octet token(tokenStr);
    Octet clientSecret(Octet::TOKEN_SIZE);
    Octet u(Octet::TOKEN_SIZE);
    Octet ut(Octet::TOKEN_SIZE);

    // Authentication pass 1
    int res = MPIN_CLIENT_1(date, &cid, &rng, &x, pin.GetHash(), &token, &clientSecret, &u, &ut, &timePermit);
    if(res)
    {
        return Status(Status::CRYPTO_ERROR, String().Format("MPIN_CLIENT_1() failed with code %d", res));
    }

    KILL_CSPRNG(&rng);

    commitmentU = u.ToString();
    commitmentUT = ut.ToString();

    SaveDataForPass2(mpinId, clientSecret.ToString(), x.ToString());

    return Status(Status::OK);
}

Status MPinCryptoNonTee::AuthenticatePass2(UserPtr user, const String& challenge, String& validator)
{
    const String& mpinId = user->GetMPinId();

    if(!m_sessionOpened)
    {
        return Status(Status::CRYPTO_ERROR, String("Session is not opened or not initialized"));
    }

    if(mpinId != m_mpinId)
    {
        return Status(Status::CRYPTO_ERROR, String("Wrong mpinId passed for authentication pass 2"));
    }

    Octet x(m_x);
    Octet y(challenge);
    Octet v(m_clientSecret);

    int res = MPIN_CLIENT_2(&x, &y, &v);

    ForgetPass2Data();

    if(res)
    {
        return Status(Status::CRYPTO_ERROR, String().Format("MPIN_CLIENT_2() failed with code %d", res));
    }

    validator = v.ToString();

    return Status(Status::OK);
}

Status MPinCryptoNonTee::SaveRegOTT(const String& mpinId, const String& regOTT)
{
	if(!m_initialized)
    {
        return Status(Status::CRYPTO_ERROR, String("Not initialized"));
    }

	try
	{
		String mpinIdHex = util::HexEncode(mpinId);
		json::Object::iterator i = m_tokens.Find(mpinIdHex);
        if (i == m_tokens.End())
        {
			json::Object item;
			item["regOTT"] = json::String(regOTT);
            m_tokens[mpinIdHex] = item;
        }
        else
        {
			i->element["regOTT"] = json::String(regOTT);
		}
		
		m_storage->SetData(m_tokens.ToString());
		
		return Status(Status::OK);
		
	}
    catch(const json::Exception& e)
    {
		return Status(Status::STORAGE_ERROR, e.what());
    }
}

Status MPinCryptoNonTee::LoadRegOTT(const String& mpinId, OUT String& regOTT)
{
	if(!m_initialized)
    {
        return Status(Status::CRYPTO_ERROR, String("Not initialized"));
    }
	
	try
	{
		String mpinIdHex = util::HexEncode(mpinId);
		json::Object::iterator i = m_tokens.Find(mpinIdHex);
        if (i == m_tokens.End())
        {
			regOTT.clear();
			return Status(Status::OK);
        }
        
        regOTT = json::String( i->element["regOTT"] ).Value();

		return Status(Status::OK);
		
	}
    catch(const json::Exception& e)
    {
        return Status(Status::STORAGE_ERROR, e.what());
    }
}

Status MPinCryptoNonTee::DeleteRegOTT(const String& mpinId)
{
	if(!m_initialized)
    {
        return Status(Status::CRYPTO_ERROR, String("Not initialized"));
    }
	
	try
	{
		String mpinIdHex = util::HexEncode(mpinId);
		json::Object::iterator i = m_tokens.Find(mpinIdHex);
        if (i == m_tokens.End())
        {
			return Status(Status::OK);
        }
        else
        {
			json::Object& item = (json::Object&) i->element;
			i = item.Find("regOTT");
			if (i == item.End())
			{
				return Status(Status::OK);
			}
			else
			{
                std::string& regOTT = ((json::String&) i->element).Value();
                util::OverwriteString(regOTT);
				item.Erase(i);
			}
		}

		if(!m_storage->SetData(m_tokens.ToString()))
        {
            return Status(Status::STORAGE_ERROR, m_storage->GetErrorMessage());
        }
		
		return Status(Status::OK);
	}
    catch(const json::Exception& e)
    {
        return Status(Status::STORAGE_ERROR, e.what());
    }
}

bool MPinCryptoNonTee::StoreToken(const String& mpinId, const String& token)
{
    if(token.length() == 0)
    {
        return false;
    }

    String mpinIdHex = util::HexEncode(mpinId);
    try
    {
        json::Object element;
        element["token"] = json::String(util::HexEncode(token));
        m_tokens[mpinIdHex] = element;
    }
    catch(json::Exception e)
    {
        return false;
    }

    String tokensJson = m_tokens.ToString();
    if(tokensJson.length() == 0)
    {
        return false;
    }

    if(!m_storage->SetData(tokensJson))
    {
        return false;
    }

    return true;
}

void MPinCryptoNonTee::DeleteToken(const String& mpinId)
{
    try
    {
        json::Object::iterator i = m_tokens.Find(util::HexEncode(mpinId));
        if(i == m_tokens.End())
        {
            return;
        }

        m_tokens.Erase(i);
        m_storage->SetData(m_tokens.ToString());
    }
    catch(json::Exception)
    {
    }
}

String MPinCryptoNonTee::GetToken(const String& mpinId)
{
    try
    {
        json::Object::iterator i = m_tokens.Find(util::HexEncode(mpinId));
        if(i == m_tokens.End())
        {
            return "";
        }

        return util::HexDecode((std::string) ((json::String) i->element["token"]));
    }
    catch(json::Exception e)
    {
        return "";
    }
}

void MPinCryptoNonTee::SaveDataForPass2(const String& mpinId, const String& clientSecret, const String& x)
{
    ForgetPass2Data();

    m_mpinId = mpinId;
    m_clientSecret = clientSecret;
    m_x = x;
}

void MPinCryptoNonTee::ForgetPass2Data()
{
    m_mpinId.Overwrite();
    m_clientSecret.Overwrite();
    m_x.Overwrite();
}

void MPinCryptoNonTee::GenerateRandomSeed(char *buf, size_t len)
{
    // TODO: This should be changed/improved to generate some real entropy
    // TODO: seedValue from client settings must be included here
#ifndef _WIN32
    FILE *fp = fopen("/dev/urandom", "rb");
    if(fp != NULL)
    {
        size_t rc = fread(buf, 1, len, fp);
        fclose(fp);
    }
#else
    srand((unsigned int) time(NULL));
    for(size_t i = 0; i < len; ++i)
    {
        int r = (rand() * 256) / RAND_MAX;
        buf[i] = r;
    }
#endif
}
