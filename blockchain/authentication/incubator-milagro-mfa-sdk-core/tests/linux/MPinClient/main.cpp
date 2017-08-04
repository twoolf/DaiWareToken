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

#include "MpinClient.h"
#include "CvLogger.h"
#include "CvThread.h"
#include "CvTime.h"
#include "CvHttpRequest.h"
#include <cstdlib>
#include <getopt.h>

using namespace std;
using namespace CvShared;

//#define BACKEND_URL		"http://ec2-54-77-232-113.eu-west-1.compute.amazonaws.com"

typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;

void PrintUsage(const char* aExeName, const char* aMessage = NULL)
{
	printf("\n");
	if (aMessage != NULL)
	{
		printf("%s\n", aMessage);
	}
	printf("Usage: %s --reg --auth -n <num-of-clients> -r <requests-per-second> [-u <user-id> -t <timeout-sec> -c <count> -o <reg-otc>] -b <backend-url>\n", aExeName);
	printf("\n");
}

struct sParams
{
	sParams() :
		bRegister(false), bAuthenticate(false), numOfClients(0), requestsPerSecond(0),
		count(1), userId("test%d@dispostable.com"), timeout(30)
	{}

	bool		bRegister;
	bool		bAuthenticate;
	uint32_t	numOfClients;
	uint32_t	requestsPerSecond;
	uint32_t	count;
	::String	backendUrl;
	::String	userId;
	Seconds		timeout;
	::String	regOTC;
};

bool doargs(int argc, char **argv, OUT sParams& aParams)
{
	char ch;

	if (argc == 1)
	{
		PrintUsage(argv[0]);
		return false;
	}

	aParams.bRegister = false;
	aParams.bAuthenticate = false;

	static struct option long_options[] = {
		{ "reg", no_argument, NULL, 1},
		{ "auth", no_argument, NULL, 2},
		{ 0, 0, 0, 0}
	};
	int option_index = 0;

	while ((ch = getopt_long(argc, argv, "n:r:b:u:t:c:o:", long_options, &option_index)) > 0)
	{
		switch (ch)
		{
			case 'n': aParams.numOfClients = atoi(optarg);
				break;
			case 'r': aParams.requestsPerSecond = atoi(optarg);
				break;
			case 'b': aParams.backendUrl = optarg;
				break;
			case 'u': aParams.userId = optarg;
				break;
			case 't': aParams.timeout = atoi(optarg);
				break;
			case 'c': aParams.count = atoi(optarg);
				break;
			case 1: aParams.bRegister = true;
				break;
			case 2: aParams.bAuthenticate = true;
				break;
			case 'o': aParams.regOTC = optarg;
				break;
		}
	}

	return true;
}

bool SleepRandTime(uint32_t aRequestsPerSecond)
{
	Millisecs time2wait(Seconds(1).ToMillisecs() / aRequestsPerSecond);

	int randFactor = (rand() * time2wait.Value()) / RAND_MAX - time2wait.Value() / 2; // -0.5*time2wait ... +0.5time2wait

	Millisecs randTime2wait(time2wait.Value() + randFactor);

	LogMessage(enLogLevel_Info, "Waiting [%ld msec] before next request", randTime2wait.Value());

	return SleepFor(randTime2wait);
}

void WaitAllDone(std::list<CMpinClient*> aListClients, const Seconds aTimeout)
{
	CvShared::TimeValue_t i = 0;
	size_t notDone = aListClients.size();
	CvShared::TimeValue_t limit = aTimeout.ToMillisecs();
	while (notDone > 0 && i < limit)
	{
		notDone = 0;
		Millisecs time2wait = Seconds(1);
		i += time2wait.Value();

		std::list<CMpinClient*>::iterator itr = aListClients.begin();
		for (; itr != aListClients.end(); ++itr)
		{
			CMpinClient* pClient = *itr;

			if (!pClient->Done())
			{
				++notDone;
				if (i >= limit)
				{
					printf("Clients #%d for [%s] is not done yet. Tired waiting...\n", pClient->GetId(), pClient->GetUserId().c_str());
					LogMessage(enLogLevel_Warning, "Clients #%d for [%s] is not done yet. Tired waiting...", pClient->GetId(), pClient->GetUserId().c_str());
				}
			}
		}

		if (notDone > 0 && i < limit)
		{
			printf("Waiting for clients to finish (%ld/%ld). %ld out of %ld not done yet...\n", Millisecs(i).ToSeconds(), Millisecs(limit).ToSeconds(),
					notDone, aListClients.size());
			SleepFor(time2wait);
		}
	}
}

int main(int argc, char** argv)
{
	InitLogger(argv[0], enLogLevel_Debug1);
	LogMessage(enLogLevel_Info, "========== Starting M-Pin Client Test ==========");

	sParams params;

	if (!doargs(argc, argv, params))
	{
		return 102;
	}

	if (params.numOfClients == 0)
	{
		PrintUsage(argv[0], "Missing parameter: -n <num-of-clients>");
		return -1;
	}

	if (params.requestsPerSecond == 0)
	{
		PrintUsage(argv[0], "Missing parameter: -r <requests-per-second>");
		return -1;
	}

	if (params.backendUrl.empty())
	{
		PrintUsage(argv[0], "Missing parameter: -b <backend-url>");
		return -1;
	}

	CvHttpRequest::COpenSslMt sslMtLock;

	srand(time(NULL));

	std::list<CMpinClient*> listClients;

	for (int i = 0; i < params.numOfClients; ++i)
	{
		::String userId;
		if ( params.userId.find("%d") != ::String::npos )
		{
			userId.Format( params.userId.c_str(), i + 1 );
		}
		else
		{
			userId = params.userId;
		}
	

		CMpinClient* pClient = NULL;

		if (params.bRegister)
		{
			::String pinGood; pinGood.Format("%04d", rand() % 10000);
			::String pinBad; pinBad.Format("%04d", rand() % 10000);

			while (pinBad == pinGood)
			{
				pinBad.Format("%04d", rand() % 10000);
			}

			pClient = new CMpinClient(i + 1, params.backendUrl, userId, pinGood, pinBad, params.regOTC);
		}
		else
		{
			pClient = new CMpinClient(i + 1, params.backendUrl, userId);
		}

		listClients.push_back(pClient);
	}

	std::list<CMpinClient*>::iterator itr;

	for (int j = 0; j < params.count; ++j)
	{
		if (params.bRegister)
		{
			for (itr = listClients.begin(); itr != listClients.end(); ++itr)
			{
				CMpinClient* pClient = *itr;

				if (itr != listClients.begin())
				{
					SleepRandTime(params.requestsPerSecond);
				}

				pClient->Register();
			}
		}

		//	SleepFor( Seconds(5) );
		//	printf( "Hit any key to continue with authentication..." );
		//	getchar();

		if (params.bAuthenticate)
		{
			//First Authentication will retrieve Time Permits, while the second will work with cached ones.
//			for (int i = 0; i < 2; ++i)
//			{
			for (itr = listClients.begin(); itr != listClients.end(); ++itr)
			{
				CMpinClient* pClient = *itr;

				//				pClient->EnableStats(i>0);

				if (itr != listClients.begin())
				{
					SleepRandTime(params.requestsPerSecond);
				}

				bool bAuthBad = ((rand() % 10) == 0);

				if (bAuthBad)
					pClient->AuthenticateBad();
				else
					pClient->AuthenticateGood();
			}

//			if ( i == 0 )
//			{
//				WaitAllDone( listClients );
//				printf( "Hit any key to continue with authentication..." );
//
//				getchar();
//			}
//		}
		}
	}

	SleepFor(Seconds(5));
	WaitAllDone(listClients, 30 + params.count * 10);

	printf("==============================================================================================\n");
	printf("Client ID | User ID | # Errors | # Regs | Min.Reg.Time | Max.Reg.Time | Avg.Reg.Time | # Auths | Min.Auth.Time | Max.Auth.Time | Avg.Auth.Time (all times in are in msec)\n");
	printf("----------------------------------------------------------------------------------------------\n");

	CMpinClient::sStats_t total;

	for (itr = listClients.begin(); itr != listClients.end(); ++itr)
	{
		CMpinClient* pClient = *itr;
		const CMpinClient::sStats_t& stats = pClient->GetStats();
		printf(" #%d | %s | %d | %d | %d | %d | %d | %d | %d | %d | %d\n",
				pClient->GetId(), pClient->GetUserId().c_str(), stats.m_numOfErrors,
				stats.m_numOfReg, stats.m_minRegMsec, stats.m_maxRegMsec, stats.m_avgRegMsec,
				stats.m_numOfAuth, stats.m_minAuthMsec, stats.m_maxAuthMsec, stats.m_avgAuthMsec);

		total.m_numOfErrors += stats.m_numOfErrors;

		if (stats.m_numOfErrors == 0)
		{
			if (stats.m_numOfReg > 0)
			{
				total.m_avgRegMsec = (total.m_avgRegMsec * total.m_numOfReg + stats.m_avgRegMsec * stats.m_numOfReg) / (total.m_numOfReg + stats.m_numOfReg);
				total.m_numOfReg += stats.m_numOfReg;
			}

			if (stats.m_numOfAuth > 0)
			{
				total.m_avgAuthMsec = (total.m_avgAuthMsec * total.m_numOfAuth + stats.m_avgAuthMsec * stats.m_numOfAuth) / (total.m_numOfAuth + stats.m_numOfAuth);
				total.m_numOfAuth += stats.m_numOfAuth;
			}
		}

		if (stats.m_minRegMsec < total.m_minRegMsec || total.m_minRegMsec == 0)
			total.m_minRegMsec = stats.m_minRegMsec;
		if (stats.m_maxRegMsec > total.m_maxRegMsec)
			total.m_maxRegMsec = stats.m_maxRegMsec;
		if (stats.m_minAuthMsec < total.m_minAuthMsec || total.m_minAuthMsec == 0)
			total.m_minAuthMsec = stats.m_minAuthMsec;
		if (stats.m_maxAuthMsec > total.m_maxAuthMsec)
			total.m_maxAuthMsec = stats.m_maxAuthMsec;
	}

	printf(" TOTAL: ======================================================================================\n");
	printf(" # Errors | # Regs | Min.Reg.Time | Max.Reg.Time | Avg.Reg.Time | # Auths | Min.Auth.Time | Max.Auth.Time | Avg.Auth.Time (all times in are in msec)\n");
	printf("----------------------------------------------------------------------------------------------\n");
	printf("  %d | %d | %d | %d | %d | %d | %d | %d | %d\n", total.m_numOfErrors,
			total.m_numOfReg, total.m_minRegMsec, total.m_maxRegMsec, total.m_avgRegMsec,
			total.m_numOfAuth, total.m_minAuthMsec, total.m_maxAuthMsec, total.m_avgAuthMsec);
	printf("==============================================================================================\n");

	printf("Terminating clients...\n");

	for (itr = listClients.begin(); itr != listClients.end(); ++itr)
	{
		CMpinClient* pClient = *itr;

		delete pClient;
	}

	if (total.m_numOfErrors > 0)
	{
		printf("Exiting with %d errors :(\n", total.m_numOfErrors);
	}
	else
	{
		printf("Exiting without errors :)\n");
	}

	LogMessage(enLogLevel_Info, "========== M-Pin Test Client Done ==========");

	return ( total.m_numOfErrors > 0) ? -1 : 0;
}
