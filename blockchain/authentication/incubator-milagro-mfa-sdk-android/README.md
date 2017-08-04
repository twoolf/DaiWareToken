# Milagro Mobile SDK for Android

## Building the Milagro Mobile SDK for Android

### Prerequisites

1. Download and install Android Studio or higher with Android SDK 16 or higher
1. Download or Clone the project and its submodule to \<milagro-sdk-android\>

### Building the Milagro Mobile SDK

#### From Android Studio
1. Import the project - File-> Open -> \<milagro-sdk-android\>
1. From Gradle Tool View select :mpinsdk -> Tasks -> build -> build
1. The assembled aars will be located in \<milagro-sdk-android\>/mpinsdk/build/outputs/aar
 
#### From Command Line
1. Navigate to \<milagro-sdk-android\>
1. Execute ./gradlew build
1. The assembled aars will be located in \<milagro-sdk-android\>/mpinsdk/build/outputs/aar

For further details, see [Milagro Mobile SDK for Android Documentation](http://docs.milagro.io/en/mfa/mobile-sdk-android/milagro-mfa-mobile-sdk-developer-guide.html)

## SDK API for Android (`com.miracl.mpinsdk.MPinSDK`)

The Android SDK API is used by Android application developers for integrating with the SDK. The API resembles the SDK Core layer, but it exposes to the Application layer, only those methods that the application needs. Most of the methods return a `Status` object which is defined as follows:

```java
public class Status {

    public enum Code {
        OK,
        CANCELED_BY_USER,        // Local error, returned when user cancels pin entering
        CRYPTO_ERROR,            // Local error in crypto functions
        STORAGE_ERROR,           // Local storage related error
        NETWORK_ERROR,           // Local error - cannot connect to remote server (no internet, or invalid server/port)
        RESPONSE_PARSE_ERROR,    // Local error - cannot parse json response from remote server (invalid json or unexpected json structure)
        FLOW_ERROR,              // Local error - improper MPinSDK class usage
        IDENTITY_NOT_AUTHORIZED, // Remote error - the remote server refuses user registration
        IDENTITY_NOT_VERIFIED,   // Remote error - the remote server refuses user registration because identity is not verified
        REQUEST_EXPIRED,         // Remote error - the register/authentication request expired
        REVOKED,                 // Remote error - cannot get time permit (probably the user is temporary suspended)
        INCORRECT_PIN,           // Remote error - user entered wrong pin
        INCORRECT_ACCESS_NUMBER, // Remote/local error - wrong access number (checksum failed or RPS returned 412)
        HTTP_SERVER_ERROR,       // Remote error, that was not reduced to one of the above - the remote server returned internal server error status (5xx)
        HTTP_REQUEST_ERROR,      // Remote error, that was not reduced to one of the above - invalid data sent to server, the remote server returned 4xx error status
        BAD_USER_AGENT,          // Remote error - user agent not supported
        CLIENT_SECRET_EXPIRED    // Remote error - re-registration required because server master secret expired
    }

    public Status(int statusCode, String error) {
        ...
    }

    public Code getStatusCode() {
        ...
    }

    public String getErrorMessage() {
        ...
    }

    @Override
    public String toString() {
        ...
    }

    ...
}
```

##### `MPinSDK()`
This method constructs an SDK instance.

##### `Status Init(Map<String, String> config, Context context)`
##### `Status Init(Map<String, String> config, Context context, Map<String, String> customHeaders)`
This method initializes the SDK. It receives a key/value map of the configuration parameters.
The configuration is a key-value map into which different configuration options can be inserted. This is a flexible way of passing configurations to the SDK, as the method parameters will not change when new configuration parameters are added. 
Unsupported parameters are ignored. Currently, the SDK recognizes the following parameters:

* `backend` - the URL of the Milagro MFA back-end service (Mandatory)
* `rpsPrefix` - the prefix that should be added for requests to the RPS (Optional). The default value is `"rps"`.

The `customHeaders` parameter is optional and allows the caller to pass additional map of custom headers, which will be added to any HTTP request that the SDK executes.

##### `void SetClientId(String clientId)`
This method will set a specific _Client ID_ which the SDK should use when sending requests to the backend.
As an example, the MIRACL MFA Platform issues _Client IDs_ for registered applications, which use the platform for authenticating users.
When the SDK is used to authenticate users specifically for this registered application, the _Client ID_ should be set by the app using this method. 

##### `Status TestBackend(String server)`
##### `Status TestBackend(String server, String rpsPrefix)`
This method will test whether `server` is a valid back-end URL by trying to retrieve Client Settings from it.
Optionally, a custom RPS prefix might be specified if it was customized at the back-end and is different than the default `"rps"`.
If the back-end URL is a valid one, the method will return Status `OK`.

##### `Status SetBackend(String server)`
##### `Status SetBackend(String server, String rpsPrefix)`
This method will change the currently configured back-end in the SDK.
Initially the back-end might be set through the `Init()` method, but then it might be change using this method.
`server` is the new back-end URL that should be used.
Optionally, a custom RPS prefix might be specified if it was customized at the back-end and is different than the default `"rps"`.
If successful, the method will return Status `OK`.

##### `User MakeNewUser(String id)`
##### `User MakeNewUser(String id, String deviceName)`
This method creates a new `User` object. The User object represents an end-user of the Milagro authentication.
The user has its own unique identity, which is passed as the id parameter to this method.
Additionally, an optional `deviceName` might be specified. The _Device Name_ is passed to the RPA, which might store it and use it later to determine which _M-Pin ID_ is associated with this device.
The returned value is a newly created `User` instance. The User class itself looks like this:

```java
public class User implements Closeable {
 
    public enum State {
        INVALID,
        STARTED_REGISTRATION,
        ACTIVATED,
        REGISTERED,
        BLOCKED
    };
     
    public String getId() {
        ...
    }
 
    public State getState() {
        ...
    }

    public String getBackend() {
        ...
    }
     
    @Override
    public String toString() {
        return getId();
    }
 
    ...
}
```

The newly created user is in the `INVALID` state.

##### `void DeleteUser(User user)`
This method deletes a user from the users list that the SDK maintains.
All the user data including its _M-Pin ID_, its state and _M-Pin Token_ will be deleted.
A new user with the same identity can be created later with the `MakeNewUser()` method.

##### `void ListUsers(List<User> users)`
This method populates the provided list with all the users that are associated with the currently set backend.
Different users might be in different states, reflecting their registration status.
The method will return Status `OK` on success and `FLOW_ERROR` if no backend is set through the `Init()` or `SetBackend()` methods.

##### `Status ListUsers(List<User> users, String backend)`
This method populates the provided list with all the users that are associated with the provided `backend`.
Different users might be in different states, reflecting their registration status.
The method will return Status `OK` on success and `FLOW_ERROR` if the SDK was not initialized.

##### `Status ListAllUsers(List<User> users)`
This method populates the provided list with all the users associated with all the backends know to the SDK.
Different users might be in different states, reflecting their registration status.
The user association to a backend could be retrieved through the `User.getBackend()` method.
The method will return Status `OK` on success and `FLOW_ERROR` if the SDK was not initialized.

##### `Status ListBackends(List<String> backends)`
This method will populate the provided list with all the backends known to the SDK.
The method will return Status `OK` on success and `FLOW_ERROR` if the SDK was not initialized.

##### `Status GetServiceDetails(String url, ServiceDetails serviceDetails)`
This method is provided for applications working with the _MIRACL MFA Platform_.
After scanning a QR Code from the platform login page, the app should extract the URL from it and call this method to retreive the service details.
The service details include the _backend URL_ which needs to be set back to the SDK in order connect it to the platform.
This method could be called even before the SDK has been initialized, or alternatively the SDK could be initialized without setting a backend, and `SetBackend()` could be used after the backend URL has been retreived through this method.
The returned `ServiceDetails` look as follows:
```java
public class ServiceDetails {
    public String name;
    public String backendUrl;
    public String rpsPrefix;
    public String logoUrl;
}
 ```
* `name` is the service readable name
* `backendUrl` is the URL of the service backend. This URL has to be set either via the SDK `Init()` method or using  `SetBackend()`
* `rpsPrefix` is RPS prefix setting which is also provided together with `backendUrl` while setting a backend
* `logoUrl` is the URL of the service logo. The logo is a UI element that could be used by the app.

##### `Status GetSessionDetails(String accessCode, SessionDetails sessionDetails)`
This method could be optionally used to retrieve details regarding a browser session when the SDK is used to authenticate users to an online service, such as the _MIRACL MFA Platform_.
In this case an `accessCode` is transferred to the mobile device out-of-band e.g. via scanning a graphical code. The code is then provided to this method to get the session details.
This method will also notify the backend that the `accessCode` was retrieved from the browser session.
The returned `SessionDetails` look as follows:
```java
public class SessionDetails {

    public String prerollId;
    public String appName;
    public String appIconUrl;
}
```
During the online browser session an optional user identity might be provided meaning that this is the user that wants to register/authenticate to the online service.
* The `prerollId` will carry that user ID, or it will be empty if no such ID was provided.
* `appName` is the name of the web application to which the service will authenticate the user.
* `appIconUrl` is the URL from which the icon for web application could be downloaded.

##### `Status StartRegistration(User user)`
##### `Status StartRegistration(User user, String activateCode)`
##### `Status StartRegistration(User user, String activateCode, String userData)`
This method initializes the registration for a User that has already been created. The SDK starts the Milagro Setup flow, sending the necessary requests to the back-end service.
The State of the User instance will change to `STARTED_REGISTRATION`. The status will indicate whether the operation was successful or not.
During this call, an _M-Pin ID_ for the end-user will be issued by the RPS and stored within the user object.
The RPA could also start a user identity verification procedure, by sending a verification e-mail.

The optional `activateCode` parameter might be provided if the registration process requires such.
In cases when the user verification is done through a _One-Time-Code_ (OTC) or through an SMS that carries such code, this OTC should be passed as the `activateCode` parameter.
In those cases, the identity verification should be completed instantly and the User State will be set to `ACTIVATED`.
 
Optionally, the application might pass additional `userData` which might help the RPA to verify the user identity.
The RPA might decide to verify the identity without starting a verification process. In this case, the Status of the call will still be `OK`, but the User State will be set to `ACTIVATED`.

##### `Status RestartRegistration(User user)`
##### `Status RestartRegistration(User user, String userData)`
This method re-initializes the registration process for a user, where registration has already started.
The difference between this method and `StartRegistration()` is that it will not generate a new _M-Pin ID_, but will use the one that was already generated.
Besides that, the methods follow the same procedures, such as getting the RPA to re-start the user identity verification procedure of sending a verification email to the user.

The application could also pass additional `userData` to help the RPA to verify the user identity.
The RPA might decide to verify the identity without starting a verification process. In this case, the Status of the call will still be `OK`, but the User State will be set to `ACTIVATED`.

##### `Status ConfirmRegistration(User user)`
##### `Status ConfirmRegistration(User user, String pushMessageIdentifier)`
This method allows the application to check whether the user identity verification process has been finalized or not.
The provided `user` object is expected to be either in the `STARTED_REGISTRATION` state or in the `ACTIVATED` state.
The latter is possible if the RPA activated the user immediately with the call to `StartRegistration()` and no verification process was started.
During the call to `ConfirmRegistration()` the SDK will make an attempt to retrieve _Client Key_ for the user.
This attempt will succeed if the user has already been verified/activated but will fail otherwise.
The method will return Status `OK` if the Client Key has been successfully retrieved and `IDENTITY_NOT_VERIFIED` if the identity has not been verified yet.
If the method has succeeded, the application is expected to get the desired PIN/secret from the end-user and then call `FinishRegistration()`, and provide the PIN.

**Note** Using the optional parameter `pushMessageIdentifier`, the application can provide a platform specific identifier for sending _Push Messages_ to the device. Such push messages might be utilized as an alternative to the _Access Number/Code_, as part of the authentication flow.

##### `Status FinishRegistration(User user, String pin)`
This method finalizes the user registration process.
It extracts the _M-Pin Token_ from the _Client Key_ for the provided `pin` (secret), and then stores the token in the secure storage.
On successful completion, the User state will be set to `REGISTERED` and the method will return Status `OK`.

##### `Status StartAuthentication(User user)`
##### `Status StartAuthentication(User user, String accessCode)`
This method starts the authentication process for a given `user`.
It attempts to retrieve the _Time Permits_ for the user, and if successful, will return Status `OK`.
If they cannot be retrieved, the method will return Status `REVOKED`.
If this method is successfully completed, the app should read the PIN/secret from the end-user and call one of the `FinishAuthentication()` variants to authenticate the user.

Optionally, an `accessCode` could be provided. This code is retrieved out-of-band from a browser session when the user has to be authenticated to an online service, such as the _MIRACL MFA Platform_.
When this code is provided, the SDK will notify the service that authentication associated with the given `accessCode` has started for the provided user. 

##### `Status CheckAccessNumber(String accessNumber)`
This method is used only when a user needs to be authenticated to a remote (browser) session, using _Access Number_.
The access numbers might have a check-sum digit in them and this check-sum needs to be verified on the client side, in order to prevent calling the back-end with non-compliant access numbers.
The method will return Status `OK` if successful, and `INCORRECT_ACCESS_NUMBER` if not successful.

##### `Status FinishAuthentication(User user, String pin)`
##### `Status FinishAuthentication(User user, String pin, StringBuilder authResultData)`
This method performs end-user authentication where the `user` to be authenticated is passed as a parameter, along with his `pin` (secret).
The method performs the authentication against the _Milagro MFA Server_ using the provided PIN and the stored _M-Pin Token_, and then logs into the RPA.
The RPA responds with the authentication _User Data_ which is returned to the application through the `authResultData` parameter.
If successful, the returned status will be `OK`, and if the authentication fails, the return status would be `INCORRECT_PIN`.
After the 3rd (configurable in the RPS) unsuccessful authentication attempt, the method will return `INCORRECT_PIN` and the User State will be set to `BLOCKED`.

##### `Status FinishAuthenticationOTP(User user, String pin, OTP otp)`
This method performs end-user authentication for an OTP. The authentication process is similar to `FinishAuthentication()`, but the RPA issues an OTP instead of logging the user into the application.
The returned status is analogical to the `FinishAuthentication()` method, but in addition to that, an `OTP` object is returned. The `OTP` class looks like this:
```java
public class OTP {
    public String otp;
    public long expireTime;
    public int ttlSeconds;
    public long nowTime;
    public Status status;
}
```
* The `otp` string is the issued OTP.
* The `expireTime` is the Milagro MFA system time when the OTP will expire.
* The `ttlSeconds` is the expiration period in seconds.
* The `nowTime` is the current Milagro MFA system time.
* `status` is the status of the OTP generation. The status will be `OK` if the OTP was successfully generated, or `FLOW_ERROR` if not.

**NOTE** that OTP might be generated only by RPA that supports that functionality, such as the MIRACL M-Pin SSO. Other RPA's might not support OTP generation where the `status` inside the returned `otp` instance will be `FLOW_ERROR`.

##### `Status FinishAuthenticationAN(User user, String pin, String accessNumber)`
This method authenticates the end-user using an _Access Number_ (also refered as _Access Code_), provided by a PC/Browser session.
After this authentication, the end-user can log into the PC/Browser which provided the Access Number, while the authentication itself is done on the Mobile Device.
`accessNumber` is the Access Number from the browser session. The returned status might be:
* `OK` - Successful authentication.
* `INCORRECT_PIN` - The authentication failed because of incorrect PIN. After the 3rd (configurable in the RPS) unsuccessful authentication attempt, the method will still return `INCORRECT_PIN` but the User State will be set to `BLOCKED`.
* `INCORRECT_ACCESS_NUMBER` - The authentication failed because of incorrect Access Number. 

##### `Status FinishAuthenticationMFA(User user, String pin, StringBuilder authzCode)`
This method is almost identical to the standard `FinishAuthentication()`, but it returns back an _Authorization Code_, which should be used further by the app back-end to validate the authenticated user.
This method is useful when authenticating users against the MIRACL MFA Platform.
For this flow to work, the app should also set a _Client ID_ through the `SetClientId()` method.
The Platform will provide the _Authorization Code_ as a result from the authentication.
This code should be then passed by the app to the back-end, where it should be verified using one of the MFA Paltform SDK flavors.

##### `bool CanLogout(User user)`
This method is used after authentication with an Access Number/Code through `FinishAuthenticationAN()`.
After such an authentication, the Mobile Device can log out the end-user from the Browser session, if the RPA supports that functionality.
This method checks whether logout information was provided by the RPA and the remote (Browser) session can be terminated from the Mobile Device.
The method will return `true` if the user can be logged-out from the remote session, and `false` otherwise.

##### `bool Logout(User user)`
This method tries to log out the end-user from a remote (Browser) session after a successful authentication through `FinishAuthenticationAN()`.
Before calling this method, it is recommended to ensure that logout data was provided by the RPA and that the logout operation can be actually performed.
The method will return `true` if the logged-out request to the RPA was successful, and `false` otherwise.

##### `String GetClientParam(String key)`
This method returns the value for a _Client Setting_ with the given key.
The value is returned as a String always, i.e. when a numeric or a boolean value is expected, the conversion should be handled by the application. 
Client settings that might interest the applications are:
* `accessNumberDigits` - The number of Access Number digits that should be entered by the user, prior to calling `FinishAuthenticationAN()`.
* `setDeviceName` - Indicator (`true/false`) whether the application should ask the user to insert a _Device Name_ and pass it to the `MakeNewUser()` method.
* `appID` - The _App ID_ used by the backend. The App ID is a unique ID assigned to each customer or application. It is a hex-encoded long numeric value. The App ID can be used only for information purposes and it does not affect the application's behavior in any way.

For more information you can refer to the [SDK Core](https://github.com/apache/incubator-milagro-mfa-sdk-core)