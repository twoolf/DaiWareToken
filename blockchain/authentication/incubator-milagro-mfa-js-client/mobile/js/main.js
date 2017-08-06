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

var mpin = mpin || {};

(function() {
    var lang = {}, hlp = {};
    var loader;
    var IMAGES_PATH = "resources/templates/@@templatename/img/";

    //CONSTRUCTOR
    mpin = function(domID, options) {
        var self = this;

        loader("js/handlebars.runtime.min.js", function() {
            loader("js/mpin-all.min.js", function() {
                loader("js/templates.js", function() {
                    var _options = {};
                    if (!options.clientSettingsURL) {
                        return console.error("set client Settings");
                    }

                    //remove _ from global SCOPE
                    _options.client = options;

                    self.ajax(options.clientSettingsURL, function(serverOptions) {
                        if(serverOptions.error === 500) {

                            _options.server = '';
                            document.getElementById(domID).innerHTML = mpin._.template(mpin.template['offline'], {});
                            return;
                        }
                        _options.server = serverOptions;
                        self.initialize.call(self, domID, _options);
                    });
                });
            });
        });

    };

    //CONFIGS
    mpin.cfg = {
//      apiVersion: "v0.3",
//      apiUrl: "https://m-pinapi.certivox.net/",
//      apiUrl: "http://dtatest.certivox.me/",
        language: "en",
        pinSize: 4,
        requiredOptions: "appID; signatureURL; mpinAuthServerURL; timePermitsURL; seedValue",
        defaultOptions: {
            identityCheckRegex: /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/,
            setDeviceName: false,
            accessNumberUseCheckSum: true
        },
        expireOtpSeconds: 99,
        touchevents: true
    };

    /**
     * Mpin Constructor
     *
     * @param {type} domID PinPad element ID
     * @param {type} options
     *
     * @returns {Boolean}
     */
    mpin.prototype.initialize = function(domID, options) {
        this.el = document.getElementById(domID);
        this.elHelp = document.getElementById('helpContainer');
        this.elHelpOverlay = document.getElementsByTagName("help")[0];
        this.elHelpHub = document.getElementsByTagName("helpHub")[0];
        this.accessNumber = "";
        this.currentDate = null;

        // Register handlebars helper

        Handlebars.registerHelper("hlp", function(optionalValue) {
            return hlp.text(optionalValue);
        });

        Handlebars.registerHelper("img", function(imgSrc) {
            return hlp.img(imgSrc);
        });

        Handlebars.registerHelper("loop", function (n, block) {
            var accum = '';
            for (var i = 0; i < n; ++i)
                accum += block.fn(i);
            return accum;
        });

        //options CHECK
        if (!options || !this.checkOptions(options.server)) {
            return console.error("Some options are required: " + mpin.cfg.requiredOptions);
        }

        //Extend string with extra methods
        setStringOptions();

        //data Source
        this.ds = this.dataSource();

        //set Options
        this.setDefaults().setOptions(options.server).setOptions(options.client);


		if (!this.opts.certivoxURL.mpin_endsWith("/")) {
			this.opts.certivoxURL += "/";
		}

        //if set & exist
        if (this.opts.language && lang[this.opts.language]) {
            this.language = this.opts.language;
        } else {
            this.language = mpin.cfg.language;
        }
        this.setLanguageText();

        // Prevent user from scrolling on touch

        document.ontouchmove = function(e){ e.preventDefault(); }

        this.renderHomeMobile();

        // Simulate OTP
        // var authData = {};
        //    authData._mpinOTP = 99;
        //    authData.expireTime = 1414593174295000;
        //    authData.nowTime = 1414593174195000;

        //    this.renderOtp(authData);

    };


    // check minimal required Options
    //  which should be set up
    mpin.prototype.checkOptions = function(options) {
        var _opts;
        _opts = mpin.cfg.requiredOptions.split("; ");
        for (var k = 0, l = _opts.length; k < l; k++) {
            if (typeof options[_opts[k]] === "undefined") {
                return false;
            }
        }
        return true;
    };

    //set defaults OPTIONS
    mpin.prototype.setDefaults = function() {
        this.opts || (this.opts = {});
        for (var i in mpin.cfg.defaultOptions) {
            this.opts[i] = mpin.cfg.defaultOptions[i];
        }

        this.opts.useWebSocket = ('WebSocket' in window && window.WebSocket.CLOSING === 2);

        return this;
    };

    mpin.prototype.setOptions = function(options) {
        var _i, _opts, _optionName, _options = "stage; allowAddUser; requestOTP; successSetupURL; onSuccessSetup; successLoginURL; onSuccessLogin; onLoaded; onGetPermit; ";
        _options += "onReactivate; onAccountDisabled; onUnsupportedBrowser; prerollid; onError; onGetSecret; mpinDTAServerURL; signatureURL; verifyTokenURL; certivoxURL; ";
        _options += "mpinAuthServerURL; registerURL; accessNumberURL; mobileAppFullURL; authenticateHeaders; authTokenFormatter; accessNumberRequestFormatter; ";
        _options += "registerRequestFormatter; onVerifySuccess; mobileSupport; emailCheckRegex; seedValue; appID; useWebSocket; setupDoneURL; timePermitsURL; timePermitsStorageURL; authenticateURL; ";
        _options += "language; customLanguageTexts; accessNumberDigits; mobileAuthenticateURL; setDeviceName; getAccessNumberURL; cSum; accessNumberUseCheckSum";

        _opts = _options.split("; ");
        this.opts || (this.opts = {});

        for (var _i = 0, _l = _opts.length; _i < _l; _i++) {
            _optionName = _opts[_i];
            if (typeof options[_optionName] !== "undefined")
                this.opts[_optionName] = options[_optionName];
        }

		MPINAuth.hash_val = this.opts.seedValue;

        if (this.opts.mpinAuthServerURL.mpin_startsWith("http")) {
            this.opts.useWebSocket = false;
        }

        if (this.opts.mpinAuthServerURL.mpin_startsWith("/")) {
            var loc = window.location;
            var newAuthServerURL;
            if (this.opts.useWebSocket) {
                newAuthServerURL = (loc.protocol === "https:") ? "wss:" : "ws:";
            } else {
                newAuthServerURL = loc.protocol;
            }

            newAuthServerURL += "//" + loc.host + this.opts.mpinAuthServerURL;
            this.opts.mpinAuthServerURL = newAuthServerURL;
        }

        this.opts.mpinAuthServerURL = (this.opts.mpinAuthServerURL.mpin_endsWith("/")) ? this.opts.mpinAuthServerURL.slice(0, this.opts.mpinAuthServerURL.length-1) : this.opts.mpinAuthServerURL;

        return this;
    };

    //return readyHtml
    mpin.prototype.readyHtml = function(tmplName, tmplData) {
        var data = tmplData, html;
        html = mpin.templates[tmplName]({data:data, cfg: mpin.cfg});
        return html;
    };


    mpin.prototype.readyHelpHub= function(tmplName, tmplData) {
        var data = tmplData, html;
        html = mpin.templates[tmplName]({data:data, cfg: mpin.cfg});
        return html;
    };


    mpin.prototype.render = function(tmplName, callbacks, tmplData) {
        var data = tmplData || {}, k;
        this.el.innerHTML = this.readyHtml(tmplName, data);

        for (k in callbacks) {

            if (document.getElementById(k) && k !== 'menuBtn') {

                if (window.navigator.msPointerEnabled) {
                    document.getElementById(k).addEventListener("MSPointerDown", callbacks[k], false);
                }
                else {

                    if(mpin.cfg.touchevents) {
                        document.getElementById(k).addEventListener('touchstart', callbacks[k], false);
                    } else {
                        document.getElementById(k).addEventListener('click', callbacks[k], false);
                    }
                }

            } else if(document.getElementById(k) && k === 'menuBtn') {
                document.getElementById('menuBtn').addEventListener('click', callbacks[k], false);
            }
        }

    };

    mpin.prototype.renderHelpHub = function(tmplName, tmplData) {
        var data = tmplData || {}, k, self = this, helphubBtns = {};

        // // Dissmiss any open help menus

        self.dismissHelp();

        this.elHelpHub.style.display = 'flex';
        this.elHelpHub.style.opacity = "1";
        this.elHelpHub.innerHTML = this.readyHelpHub(tmplName, data);

        helphubBtns.first = function(evt) {
            // Modify the sequence for the templates
            // self.renderHelp("help-helphub", callbacks);
        };

        helphubBtns.second = function(evt) {
            // Modify the sequence for the templates
            // self.renderHelp("help-helphub", callbacks);
        };

        helphubBtns.details = function(evt) {
            // Modify the sequence for the templates
            self.renderHelpHub("helphub-details");
        };

        helphubBtns.forth = function(evt) {
            // Modify the sequence for the templates
            // self.renderHelp("help-helphub", callbacks);
        };

        helphubBtns.enter = function(evt) {
            self.renderHelpHub("helphub-index");
        }

        helphubBtns.exit = function(evt) {
            self.dismissHelpHub();
        };

        for (k in helphubBtns) {
            if (document.getElementById(k)) {

                if (window.navigator.msPointerEnabled) {
                    document.getElementById(k).addEventListener("MSPointerDown", helphubBtns[k], false);
                }
                else {

                    if(mpin.cfg.touchevents) {
                        document.getElementById(k).addEventListener('touchstart', helphubBtns[k], false);
                    } else {

                        document.getElementById(k).addEventListener('click', helphubBtns[k], false);
                    }

                }

            }
        }

    };

    mpin.prototype.dismissHelp = function() {
            this.elHelpOverlay.style.display = 'none';
            this.elHelpOverlay.style.opacity = '0';
            this.elHelp.style.display = 'none';
    }

    mpin.prototype.dismissHelpHub = function() {
            this.elHelpHub.style.display = 'none';
            this.elHelpHub.style.opacity = '0';
    }

    mpin.prototype.setLanguageText = function() {
        hlp.language = this.language;
        //      setLanguageText
        if (this.opts.customLanguageTexts && this.opts.customLanguageTexts[this.language]) {
            for (var k in this.opts.customLanguageTexts[this.language]) {
                console.log("this.opts.customLanguageTexts[this.language]", this.opts.customLanguageTexts[this.language][k]);
                if (lang[this.language][k]) {
                    lang[this.language][k] = this.opts.customLanguageTexts[this.language][k];
                }
            }
        }

    };

    mpin.prototype.renderHomeMobile = function() {

        var callbacks = {}, self = this, identity, standalone, safari, userAgent, ios;

        callbacks.mpin_authenticate = function(evt) {
            // Modify the sequence for the templates
            self.renderSetupHome.call(self);
        };

        callbacks.ok_dismiss = function(evt) {
            // Modify the sequence for the templates
            self.dismissHelp.call(self);
        };

        callbacks.show_more = function(evt) {
            // Modify the sequence for the templates
            self.renderHelp("help-helphub", callbacks);
        };

        callbacks.info = function(evt) {
            // Show the help item
            self.renderHelp("help-setup-home", callbacks);
        };

        callbacks.mp_action_setup = function(evt) {
            self.actionSetupHome.call(self);
        };

        identity = this.ds.getDefaultIdentity();
        standalone = window.navigator.standalone;
        userAgent = window.navigator.userAgent.toLowerCase(),
        safari = /safari\//.test( userAgent );
        ios = /iphone|ipod|ipad/.test( userAgent );

        // Check browsers

        function isIos7() {
            var deviceAgent = userAgent;
            return /(iphone|ipod|ipad).* os 7_/.test(deviceAgent);
        }

        function isIos8() {
            var deviceAgent = userAgent;
            return /(iphone|ipod|ipad).* os 8_/.test(deviceAgent);
        }

        function isIos6() {
            var deviceAgent = userAgent;
            return /(iphone|ipod|ipad).* os 6_/.test(deviceAgent);
        }

        function goToIdentity() {
            // Check if online

            if(!navigator.onLine) {
                self.render('offline', callbacks);
            }

            // Check if there's identity, redirect to login where 'Add to identity will appear'
            else if (identity) {

                totalAccounts = self.ds.getAccounts();
                totalAccounts = Object.keys(totalAccounts).length;

                 if (totalAccounts === 0) {
                  self.renderSetupHome();
                 } else if (totalAccounts === 1 || totalAccounts > 1) {
                  self.renderAccessNumber();
                  // self.renderLogin();
                 }

            } else {
                // Render renderSetupHome, if no identity exists
                self.renderSetupHome();
            }
        }

        // Check if Safari and if it's open as standalone app
        if(ios) {

            if ( !standalone && safari ) {

                // Check if chrome and exit

                if(userAgent.match('crios')) {
                    goToIdentity();
                    return;
                }

                // Render IOS7 view
                if(isIos7() || isIos8()) {
                    this.render('ios7-startup', callbacks);

                } else if(isIos6()) {
                    // Render the IOS6 view - the difference is in the icons
                    this.render('ios6-startup', callbacks);
                } else {
                    goToIdentity();
                }

            } else if ( standalone && !safari ) {

                goToIdentity();

            } else if ( !standalone && !safari ) {

                // In app view
                this.render('ui-webview', callbacks);
            };

        } else {

            goToIdentity();
        }

    };

    mpin.prototype.renderSetupHome = function(email, errorID) {

        var callbacks = {}, self = this, userId, descHtml, deviceName = "", deviceNameHolder = "";


        var totalAccounts = this.ds.getAccounts();
        totalAccounts = Object.keys(totalAccounts).length;

        callbacks.mp_action_home = function(evt) {
            if (totalAccounts === 0) {
             self.renderSetupHome();
            } else if (totalAccounts === 1) {
             self.renderAccessNumber();
            } else if (totalAccounts > 1) {
             self.renderAccessNumber(true);
            }
        };
        callbacks.mp_action_setup = function(evt) {
            self.actionSetupHome.call(self);
        };

        userId = (email) ? email : "";

        if (this.opts.setDeviceName) {

            //get from localStorage - already set
            if (this.ds.getDeviceName()) {
                deviceName = this.ds.getDeviceName();
                deviceNameHolder = deviceName;
            } else {
                //set only placeholder value
                deviceNameHolder = this.suggestDeviceName();
                deviceName = "";
            }
        }

        this.render("setup-home", callbacks, {userId: userId, setDeviceName: this.opts.setDeviceName});

        // Put placeholder attribute

        var inputDeviceName = document.getElementById('deviceInput')
            , inputEmail = document.getElementById('emailInput');

        inputDeviceName.placeholder = deviceNameHolder;
        inputDeviceName.value = deviceName;
        inputEmail.placeholder = hlp.text("setup_text3");

    };

    mpin.prototype.renderOtp = function (authData) {
        var callbacks = {}, self = this, leftSeconds, timerEl, timer2d, drawTimer, totalSec;

        //draw canvas Clock
        drawTimer = function (expireOn) {
            var start, diff;
            diff = totalSec - expireOn;
            start = -0.5 + ((diff / totalSec) * 2);
            start = Math.round(start * 100) / 100;
            timer2d.clearRect(0, 0, timerEl.width, timerEl.height);

            timer2d.beginPath();
            timer2d.strokeStyle = "#8588ac";
            timer2d.arc(20, 20, 18, start * Math.PI, 1.5 * Math.PI);
            timer2d.lineWidth = 5;
            timer2d.stroke();
        };

        function expire (expiresOn) {
            leftSeconds = (leftSeconds) ? leftSeconds - 1 : Math.floor((expiresOn - (new Date())) / 1000);
            if (leftSeconds > 0) {
                document.getElementById("mpin_seconds").innerHTML = leftSeconds;

                if (document.getElementById("mpTimer")) {
                    drawTimer(leftSeconds);
                }

            } else {
                clearInterval(self.intervalExpire);
                self.renderOtpExpire();
            }
        }

        callbacks.mp_action_home = function () {
            clearInterval(self.intervalExpire);
            self.renderHomeMobile.call(self);
        };

        callbacks.mpin_help = function () {
            clearInterval(self.intervalExpire);
            self.lastView = "renderOtp";
            self.renderHelpHub.call(self);
        };

        callbacks.mpin_cancel = function () {
            clearInterval(self.intervalExpire);
            self.renderLogin.call(self);
        };

        this.render("otp", callbacks);

        document.getElementById("mpinOTPNumber").innerHTML = authData._mpinOTP;

        var timeOffset = new Date() - new Date(authData.nowTime);
        var expireMSec = new Date(authData.expireTime + timeOffset);

        totalSec = Math.floor((expireMSec - (new Date())) / 1000);

        if (document.getElementById("mpTimer")) {
            timerEl = document.getElementById("mpTimer");
            timer2d = timerEl.getContext("2d");
        }

        expire(expireMSec);

        this.intervalExpire = setInterval(function () {
            expire();
        }, 1000);
    };


    mpin.prototype.renderOtpExpire = function () {
        var callbacks = {}, self = this;

        callbacks.mpin_login_now = function () {
            self.renderLogin.call(self);
        };

        this.render("otp-expire", callbacks);
    };


    mpin.prototype.suggestDeviceName = function() {
        var suggestName, platform, browser;
        platform = navigator.platform.toLowerCase();
        browser = navigator.userAgent;

        if (platform.indexOf("mac") !== -1) {
            platform = "mac";
        } else if (platform.indexOf("linux") !== -1) {
            platform = "lin";
        } else if (platform.indexOf("win") !== -1) {
            platform = "win";
        } else if (platform.indexOf("sun") !== -1) {
            platform = "sun";
        } else if (platform.indexOf("iphone") !== -1) {
            platform = "iOS";
        } else {
            platform = "__";
        }

        if (browser.indexOf("Chrome") !== -1) {
            browser = "Chrome";
        } else if (browser.indexOf("MSIE") !== -1 || browser.indexOf("Trident") !== -1) {
            browser = "Explorer";
        } else if (browser.indexOf("Firefox") !== -1) {
            browser = "Firefox";
        } else if (browser.indexOf("Safari") !== -1) {
            browser = "Safari";
        } else if (browser.indexOf("iPhone") !== -1) {
            browser = "iPhone";
        } else {
            browser = "_";
        }

        suggestName = platform + browser;

        return suggestName;
    };


    mpin.prototype.renderSetup = function(email, clientSecretShare, clientSecretParams) {

        var callbacks = {}
            , self = this;

        var totalAccounts = this.ds.getAccounts();
        totalAccounts = Object.keys(totalAccounts).length;

        callbacks.mp_action_home = function(evt) {
            if (totalAccounts === 0) {
             self.renderSetupHome();
            } else if (totalAccounts === 1) {
             self.renderAccessNumber();
            } else if (totalAccounts > 1) {
             self.renderAccessNumber(true);
            }
        };
        callbacks.mpinClear = function() {
            self.addToPin.call(self, "clear");
        };
        callbacks.mpinLogin = function() {

            if(self.pinpadInput.length < mpin.cfg.pinSize ) {

                return;
            }

            self.actionSetup.call(self);
        };
        callbacks.menuBtn = function() {
            self.toggleButton.call(self);
        };

        callbacks.show_more = function(evt) {
            // Modify the sequence for the templates
            self.renderHelpHub("helphub-index", callbacks);
        };

        callbacks.info = function(evt) {
            // Show the help item
            self.renderHelp("help-setup-home", callbacks);
        };

        this.render("setup-pin", callbacks, {email: email, pinSize: mpin.cfg.pinSize});

        this.enableNumberButtons(false);
        this.bindNumberButtons();
        this.addToPin.call(self, "clear");

        //requestSignature
        this.requestSignature(email, clientSecretShare, clientSecretParams);
    };

    mpin.prototype.renderAccessNumber = function(listAccounts) {

        var callbacks = {}
            , self = this
            , identity = this.ds.getDefaultIdentity()
            , email = this.getDisplayName(identity)
            , totalAccounts = this.ds.getAccounts();

        totalAccounts = Object.keys(totalAccounts).length;

        if (!identity) {
            this.renderSetupHome();
        }

        if (!this.identity) {
            self.setIdentity(self.ds.getDefaultIdentity(), true);

        }
        if(self.opts.requestOTP === "1") {
            self.renderLogin();
            return;
        }



        callbacks.mp_action_home = function(evt) {
			if (document.getElementById("menuBtn").classList.contains("close")) {
				document.getElementById("menuBtn").onclick();
//				self.toggleButton.call(self);
			}
			return ;
            if (totalAccounts === 0) {
             self.renderSetupHome();
            } else if (totalAccounts === 1) {
             self.renderAccessNumber();
            } else if (totalAccounts > 1) {
             self.renderAccessNumber(true);
            }
        };

        callbacks.mpinClear = function() {
            self.addToAcc.call(self, "clear", false);
        };

        callbacks.menuBtn = function() {
            self.toggleButton.call(self);
        };

        callbacks.mpinLogin = function() {

                // Validate the number of digits entered
                if(self.accessNumber.length < self.opts.accessNumberDigits ) {
                    return;
                }

				if (!self.opts.accessNumberUseCheckSum) {
					self.renderLogin.call(self);
					return;
				}

				if(self.opts.cSum === 1 && !self.checkAccessNumberValidity2(self.accessNumber, 6)) {
				    self.renderAccessNumber.call(self);
                    self.addToAcc.call(self, "clear", false);
                    self.display(hlp.text("authPin_errorInvalidAccessNumber"), true);
                    return;
				}

				if(!self.checkAccessNumberValidity(self.accessNumber, 1) && self.opts.cSum !== 1) {
                    self.renderAccessNumber.call(self);
                    self.addToAcc.call(self, "clear", false);
                    self.display(hlp.text("authPin_errorInvalidAccessNumber"), true);
                    return;
                }
                // Go to renderLogin

                self.renderLogin();
        };

        this.render("setup-access", callbacks, {email: email, menu: true});

        self.enableNumberButtons(true);
        self.enableButton(false, "go");
        self.enableButton(false, "clear");
        self.bindNumberButtons(true);

        // Clear all numbers if you logged in successfuly

        self.addToAcc.call(self, "clear", false);

        if (listAccounts) {
            this.toggleButton();
        } else {
            this.setIdentity(this.ds.getDefaultIdentity(), true, function() {
                self.display(hlp.text("pinpad_placeholder_text"));
               }, function() {
                return false;
               });
        }

    }

    mpin.prototype.renderLogin = function(listAccounts) {

        var callbacks = {}
            , self = this
            , identity = this.ds.getDefaultIdentity()
            , email = this.getDisplayName(identity)
            , totalAccounts = this.ds.getAccounts()
            , pinpadDisplay = document.getElementById("pinpad-input")

        callbacks.mp_action_home = function(evt) {
            if (totalAccounts === 0) {
             self.renderSetupHome();
            } else if (totalAccounts === 1) {
             self.renderAccessNumber();
            } else if (totalAccounts > 1) {
             self.renderAccessNumber(true);
            }
        };

        callbacks.mpinClear = function() {
            self.addToPin.call(self, "clear");
        };

        callbacks.menuBtn = function() {
            self.toggleButton.call(self);
        };

        callbacks.mpinLogin = function() {

            if(self.pinpadInput.length < mpin.cfg.pinSize ) {

                return;
            }

            self.actionLogin.call(self);
        };

        this.pinpadInput = '';
        this.render("setup", callbacks, {email: email, menu: true, pinSize: mpin.cfg.pinSize});
        this.enableNumberButtons(false);
        this.bindNumberButtons();

        // TODO

        // Help hub callbacks

        callbacks.ok_dismiss = function(evt) {
            // Modify the sequence for the templates
            self.dismissHelp.call(self);
        };

        callbacks.show_more = function(evt) {
            // Modify the sequence for the templates
            self.renderHelpHub("helphub-index");
        };

        // Helphub calbacks

        document.getElementById('openHelpHub').onclick = function(evt) {
            self.renderHelpHub("helphub-index");
        };

        if (listAccounts) {
            this.toggleButton();
        } else {
            this.setIdentity(this.ds.getDefaultIdentity(), true, function() {
                self.display(hlp.text("pinpad_placeholder_text"));
               }, function() {
                return false;
               });
        }
    };

    mpin.prototype.getAccessNumber = function() {
        var _request = new XMLHttpRequest(), self = this, expire, drawTimer, timerEl, timer2d, totalSec;

        this.intervalID || (this.intervalID = {});

        //// TIMER CODE
        if (document.getElementById("mpTimer")) {
            timerEl = document.getElementById("mpTimer");
            timer2d = timerEl.getContext("2d");
            }
        //draw canvas Clock
        drawTimer = function (expireOn) {

            var start, diff;
            diff = totalSec - expireOn;
            start = -0.5 + ((diff / totalSec) * 2);
            start = Math.round(start * 100) / 100;

            timer2d.clearRect(0, 0, timerEl.width, timerEl.height);

            timer2d.beginPath();
            timer2d.strokeStyle = "#8588ac";
            timer2d.arc(20, 20, 18, start * Math.PI, 1.5 * Math.PI);
            timer2d.lineWidth = 5;
            timer2d.stroke();
        };

        expire = function (expiresOn) {
            var expireAfter = Math.floor((expiresOn - (new Date())) / 1000);
            if (expireAfter <= 0) {
                if (self.intervalID) {
                    clearInterval(self.intervalID);
                }
                self.getAccessNumber();
            } else {
                document.getElementById("mpin_seconds").innerHTML = expireAfter;
                //////////////////////////////////////////Clockwise
                ///// Check if Timer Element exist some template did not have timer
                if (document.getElementById("mpTimer")) {
                    drawTimer(expireAfter);
                }
            }
        };

        _request.onreadystatechange = function() {
            var jsonResponse, expiresOn;
            if (_request.readyState === 4 && _request.status === 200) {
                jsonResponse = JSON.parse(_request.responseText);
                document.getElementById("mp_accessNumber").innerHTML = jsonResponse.accessNumber;
                if (jsonResponse.webOTP) {
                    self.webOTP = jsonResponse.webOTP;
                }
                expiresOn = new Date();
                totalSec = 99;
                expiresOn.setSeconds(expiresOn.getSeconds() + jsonResponse.ttlSeconds);
                expire(expiresOn);
                self.intervalID = setInterval(function() {
                    expire(expiresOn);
                }, 1000);
            }
        };
        _request.open("POST", this.opts.getAccessNumberURL);
//      _request.setRequestHeader('Content-Type', 'application/json');
        _request.send();
    };

    //post REQUEST
    mpin.prototype.getAccess = function() {
        var _request = new XMLHttpRequest(), self = this;

        _request.onreadystatechange = function() {
            var _jsonRes;
            if (_request.readyState === 4) {
                if (_request.status === 200) {
                    _jsonRes = JSON.parse(_request.responseText);
                    if (self.opts.onVerifySuccess) {
                        self.opts.onVerifySuccess(_jsonRes);
                    } else {
                        self.successLogin(_jsonRes);
                    }
                } else {
                    console.log("NOT success !!!");
                }
            }
        };

        _request.open("POST", this.opts.accessNumberURL, true);
        _request.timeout = 30000;
        _request.ontimeout = function() {
            self.getAccess();
        };
        var _sendParams = {};
        if (this.webOTP) {
            sendParams.webOTP = this.webOTP;
            _request.send(JSON.stringify(_sendParams));
        } else {
            _request.send();
        }
        return _request;
    };

    mpin.prototype.renderActivateIdentity = function() {
        var callbacks = {}, self = this, email;
        email = this.getDisplayName(this.identity);

        callbacks.mp_action_home = function(evt) {
            self.renderHomeMobile.call(self);
        };
        callbacks.mpin_action_setup = function(evt) {
            if (self.checkBtn(this))
                self.beforeRenderSetup.call(self, this);
        };
        callbacks.mpin_action_resend = function(evt) {
            if (self.checkBtn(this))
                self.actionResend.call(self, this);
        };
        //identities list
        callbacks.mpin_accounts = function() {
            self.renderAccountsBeforeSetup();
        };

        this.render("activate-identity", callbacks, {email: email});
    };

    //prevent mpin button multi clicks
    mpin.prototype.checkBtn = function(btnElem) {
        var btnClass = btnElem.className;
        return (btnClass.indexOf("mpinBtnBusy") === -1 && btnClass.indexOf("mpinBtnError") === -1 && btnClass.indexOf("mpinBtnOk") === -1);
    };

    mpin.prototype.mpinButton = function(btnElem, busyText) {

        var oldHtml = btnElem.innerHTML;
        addClass(btnElem, "mpinBtnBusy");

        btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(busyText) + "</span>";
        return {
            error: function(errorText) {
                removeClass(btnElem, "mpinBtnBusy");
                addClass(btnElem, "mpinBtnError");
                btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(errorText) + "</span>";
                setTimeout(function() {
                    removeClass(btnElem, "mpinBtnError");
                    btnElem.innerHTML = oldHtml;
                }, 1500);

            }, ok: function(okText) {
                removeClass(btnElem, "mpinBtnBusy");
                addClass(btnElem, "mpinBtnOk");
                btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(okText) + "</span>";
                setTimeout(function() {
                    removeClass(btnElem, "mpinBtnOk");
                    btnElem.innerHTML = oldHtml;
                }, 1500);
            }};
    };


    mpin.prototype.beforeRenderSetup = function(btnElem) {

        var _reqData = {}, regOTT, url, self = this;
        regOTT = this.ds.getIdentityData(this.identity, "regOTT");
        url = this.opts.signatureURL + "/" + this.identity + "?regOTT=" + regOTT;

        if (btnElem) {
                var btn = this.mpinButton(btnElem, "setupNotReady_check_info1");
        }

        _reqData.URL = url;
        _reqData.method = "GET";
        //get signature
        requestRPS(_reqData, function(rpsData) {
            if (rpsData.errorStatus) {
                btn.error("setupNotReady_check_info2");
                self.error("Activate identity");
                return;
            }
            var userId = self.getDisplayName(self.identity);
            self.renderSetup(userId, rpsData.clientSecretShare, rpsData.params);
        });


    };

//custom render
    mpin.prototype.renderAccountsPanel = function(back) {

        var self = this,
            callbacks = {},
            renderElem,
            addEmptyItem,
            c = 0,
            mpBack = document.getElementById('mp_back'),
            menuBtn = document.getElementById('menuBtn'),
            defaultIdentity;

            if(!mpBack) {
                mpBack = document.getElementById("mp_back_not_active");
            }

        if (window.navigator.msPointerEnabled) {
            menuBtn.style.bottom = '0';
        }

        if(menuBtn) {

            menuBtn.onclick = function(evt) {
                document.getElementById('accountTopBar').style.height = "";
                menuBtn.className = 'up';
                // self.renderAccessNumber();
            };

        }

        addEmptyItem = function(cnt) {
            var p = document.createElement("div");
            p.className = "mp_contentEmptyItem";
            cnt.appendChild(p);
        };

        addMpinBack = function () {

            if(document.getElementById('accountTopBar')) {
                renderElem = document.getElementById('accountTopBar').appendChild(document.createElement("div"));
                renderElem.id = "mp_back";
                mpBack = document.getElementById("mp_back");

                if(!document.getElementById("mp_back")) {
                    mpBack = document.getElementById("mp_back_not_active");
                }

                mpBack.innerHTML = self.readyHtml("accounts-panel", {});
            }
        }


        // Fix for IE compatibillity
        if(document.body.contains(mpBack) === false || document.body.contains(mpBack) === false) {

            addMpinBack();
            mpBack.style.display = 'block';

            document.getElementById("mp_acclist_adduser").onclick = function(evt) {
                self.renderSetupHome.call(self);
            };

            // Appending happens here

            var cnt = document.getElementById("mp_accountContent");
            this.addUserToList(cnt, this.ds.getDefaultIdentity(), true, 0);

            for (var i in this.ds.getAccounts()) {
                c += 1;
                if (i != this.ds.getDefaultIdentity())
                    this.addUserToList(cnt, i, false, c);
            }

            addEmptyItem(cnt);

        }

        //default IDENTITY

    };

    mpin.prototype.renderAccountsBeforeSetupPanel = function(back) {

        var self = this,
            callbacks = {},
            renderElem,
            addEmptyItem,
            c = 0,
            mpBack = document.getElementById('mp_back_not_active'),
            menuBtn = document.getElementById('menuBtn'),
            defaultIdentity;

        if (window.navigator.msPointerEnabled) {
            menuBtn.style.bottom = '0';
        }

        addEmptyItem = function(cnt) {
            var p = document.createElement("div");
            p.className = "mp_contentEmptyItem";
            cnt.appendChild(p);
        };

        addMpinBack = function () {
            renderElem = document.getElementById('identityContainer').appendChild(document.createElement("div"));
            renderElem.id = "mp_back_not_active";
            mpBack = document.getElementById("mp_back_not_active");
            mpBack.innerHTML = self.readyHtml("accounts-panel-not-active", {});
        }


        // Fix for IE compatibillity
        if(document.body.contains(mpBack) === false) {

            addMpinBack();
            mpBack.style.display = 'block';

            document.getElementById("mp_go_back").onclick = function(evt) {
                self.renderIdentityNotActive.call(self);
            };

            // Appending happens here

            var cnt = document.getElementById("mp_accountContent");
            this.addUserToList(cnt, this.ds.getDefaultIdentity(), true, 0);

            for (var i in this.ds.getAccounts()) {
                c += 1;
                if (i != this.ds.getDefaultIdentity())
                    this.addUserToList(cnt, i, false, c);
            }

            addEmptyItem(cnt);

        }

        //default IDENTITY

    };

    mpin.prototype.renderUserSettingsPanel = function(iD) {

        var renderElem, name, self = this, name = this.getDisplayName(iD), renderElemVal;

        if(document.getElementById("mp_back")) {
            renderElem = document.getElementById("mp_back");
            renderElemVal = 'mp_back';
        } else {
            renderElem = document.getElementById("mp_back_not_active");
            renderElemVal = 'mp_back_not_active';
        }

        renderElem.innerHTML = this.readyHtml("user-settings", {name: name});

        document.getElementById("mp_deluser").onclick = function(evt) {
            self.renderDeletePanel.call(self, iD);
        };
        document.getElementById("mp_reactivate").onclick = function(evt) {
            self.renderReactivatePanel.call(self, iD);
        };
        document.getElementById("mp_acclist_cancel").onclick = function(evt) {
            renderElem.parentNode.removeChild(renderElem);

            if(renderElemVal === "mp_back") {
                self.renderAccountsPanel();
            } else {
                self.renderAccountsBeforeSetupPanel();
            }
        };
    };

    mpin.prototype.renderReactivatePanel = function(iD) {
        var renderElem, name, self = this;
        name = this.getDisplayName(iD);

        if(document.getElementById("mp_back")) {
            renderElem = document.getElementById("mp_back");
        } else {
            renderElem = document.getElementById("mp_back_not_active");
        }

        renderElem.innerHTML = this.readyHtml("reactivate-panel", {name: name});

        document.getElementById("mp_acclist_reactivateuser").onclick = function() {
            self.actionSetupHome.call(self, self.getDisplayName(iD));
        };
        document.getElementById("mp_acclist_cancel").onclick = function() {
            self.renderUserSettingsPanel(iD);
        };
    };

    mpin.prototype.renderDeletePanel = function(iD) {
        var renderElem, name, self = this;
        name = this.getDisplayName(iD);

        if(document.getElementById("mp_back")) {
            renderElem = document.getElementById("mp_back");
        } else {
            renderElem = document.getElementById("mp_back_not_active");
        }

        renderElem.innerHTML = this.readyHtml("delete-panel", {name: name});

        document.getElementById("mp_acclist_deluser").onclick = function(evt) {
            self.deleteIdentity(iD);

            self.renderHomeMobile.call(self, evt);
            // Render the identity list too

        };
        document.getElementById("mp_acclist_cancel").onclick = function(evt) {
            self.renderUserSettingsPanel(iD);
        };
    };

    mpin.prototype.renderSetupDone = function() {
        var callbacks = {}, self = this, userId;

        userId = this.getDisplayName(this.identity);

        callbacks.mp_action_home = function() {
            self.renderHomeMobile.call(self);
        };
        callbacks.mp_action_go = function() {
            self.renderAccessNumber.call(self);
        };

        this.render("setup-done", callbacks, {userId: userId});
    };

    mpin.prototype.addUserToList = function(cnt, uId, isDefault, iNumber) {
        var starClass, divClass, self = this, starButton;

        if (isDefault) {
            starClass = "mp_starButtonSelectedState";
            divClass = "mp_contentItem one-edge-shadow default";
        } else {
            starClass = "mp_starButtonDefaultState";
            divClass = "mp_contentItem one-edge-shadow";
        }

        starButton = document.createElement("div");
        var name = this.getDisplayName(uId);
        starButton.setAttribute("tabindex", "-1");
        starButton.className = starClass;
        starButton.id = "mp_accountItem_" + iNumber;

        var rowElem = document.createElement("div");
        rowElem.className = divClass;
        rowElem.setAttribute("data-identity", uId);
        rowElem.appendChild(starButton);

        var tmplData = {name: name};
        rowElem.innerHTML = mpin.templates['user-row']({data:tmplData, cfg: mpin.cfg});

        cnt.appendChild(rowElem);
        rowElem.addEventListener('click', mEventsHandler, false);

        // document.getElementById('mp_back').remove();

        function mEventsHandler(e) {

            e.stopPropagation();
            e.preventDefault();

            if(document.getElementById("mp_back")) {
                var elem = document.getElementById("mp_back");
            } else {
                var elem = document.getElementById("mp_back_not_active");
            }

            elem.parentNode.removeChild(elem);

            removeClass(document.getElementsByClassName("mp_itemSelected")[0], "mp_itemSelected");
            // addClass(rowElem, "mp_itemSelected");
            self.ds.setDefaultIdentity(uId);
            self.setIdentity(uId, true);
            self.renderAccessNumber();

            // Enable pin
            self.addToAcc.call(self, "clear", false);

            // Hide the identity list

            menuBtn = document.getElementById('menuBtn');

            document.getElementById('accountTopBar').style.height = "";
            menuBtn.className = 'up';

        }

        // Append iNumber, don't use handlebars
        var innerRowElemName = "mp_btIdSettings_"
            , innerRowElem =  document.getElementById(innerRowElemName)
            , imgRowElem = hlp.img("settings.svg");

        innerRowElem.setAttribute("id",innerRowElemName + iNumber);

        document.getElementById(innerRowElemName + iNumber).onclick = function(ev) {
            console.log(uId);
            self.renderUserSettingsPanel(uId);
            ev.stopPropagation();
            return false;
        };
    };

    mpin.prototype.renderIdentityNotActive = function(email) {
        var callbacks = {}, self = this;
        var email = email? email: self.getDisplayName(this.identity);

        callbacks.mp_action_home = function(evt) {
			self.renderHomeMobile();
//            self.renderAccountsBeforeSetup();
        };

        //Check again
        callbacks.mpin_action_setup = function() {
            if (self.checkBtn(this))
                self.beforeRenderSetup.call(self, this);
        };
        //email
        callbacks.mpin_action_resend = function() {
            if (self.checkBtn(this))
                self.actionResend.call(self, this);
        };
        //identities list
        callbacks.mpin_accounts = function() {

            self.renderAccountsBeforeSetup();

        };

        this.render("identity-not-active", callbacks, {email: email});
    };

    mpin.prototype.bindNumberButtons = function(isAcc) {
        var self = this, btEls;
        btEls = document.getElementsByClassName("btn");

        function mEventsHandler(e) {

            e.stopPropagation();
            e.preventDefault();

            var parent = document.getElementById("inputContainer");
            var child = document.getElementById("codes");

            if(e.target.hasAttribute("disabled")) {
                return;
            }

            isAcc ? self.addToAcc(e.target.getAttribute("data-value"),"") : self.addToPin(e.target.getAttribute("data-value"),"");
        }

        for (var i = 0; i < btEls.length; i++) {

            // Mobile touch events

            if (window.navigator.msPointerEnabled) {

                btEls[i].addEventListener('MSPointerDown', mEventsHandler, false);

            }
            else {

                if(mpin.cfg.touchevents) {
                    btEls[i].addEventListener('touchstart', mEventsHandler, false);
                } else {

                    btEls[i].addEventListener('click', mEventsHandler, false);
                }

            }

        }
    };
    mpin.prototype.enableNumberButtons = function(enable) {

        var els = document.getElementsByClassName("btn");
        for (var i = 0; i < els.length; i++) {
            var element = els[i];
            if (enable) {
                element.className = "btn";
                element.disabled = false;
            } else {
                element.className = "btn disabled";
                element.disabled = true;
            }
        }
    };
    //
    mpin.prototype.addToPin = function(digit, iserror) {

        var digitLen
        , elemForErrcode = document.getElementById('codes')
        , self = this;

            this.pinpadInput || (this.pinpadInput = "");
            this.pinpadInput += digit;
            digitLen = this.pinpadInput.length;

            if (digit === 'login') {

                    elemForErrcode.style.display = "block";
                    elemForErrcode.innerHTML = hlp.text("pinpad_placeholder_text");
                    this.enableNumberButtons(true);

                    return;
            }

            if (digit === 'pin') {

                elemForErrcode.style.display = "block";
                elemForErrcode.innerHTML = hlp.text("pinpad_placeholder_text");

                this.enableNumberButtons(true);

                return;
            }

            if (digitLen === 1) {

                // Reset the error codes to original text
                this.resetDisplay(hlp.text("pinpad_placeholder_text"));
                this.enableButton(true, "clear");
            }

            // Append circles

            if (digitLen <= mpin.cfg.pinSize) {
                this.bindCircles();
            }

            if (digitLen === mpin.cfg.pinSize) {

                this.enableNumberButtons(false);
                this.enableButton(true, "go");
                this.enableButton(true, "clear");
            }

            if (digit === 'clear') {

                this.enableNumberButtons(true);
                this.enableButton(false, "go");
                this.enableButton(false, "clear");
                this.pinpadInput = "";
                this.clearCircles();

            }

    };

    // Add to pin ac number

    mpin.prototype.addToAcc = function(digit, iserror) {

        var accNumLen
        , elemForErrcode = document.getElementById('codes')
        , accNumHolder = document.getElementById('accNumHolder')
        , self = this;

            this.accessNumber += digit;
            accNumLen = this.accessNumber.length;
            accNumHolder.style.display = 'block';
            accNumHolder.innerHTML += digit;

            // On first click

            if (accNumLen === 1) {

                // Reset the error codes
                this.resetDisplay(hlp.text("pinpad_placeholder_text"));
                this.enableButton(true, "clear");
            }

            if (accNumLen === this.opts.accessNumberDigits) {

                // Append the number of circles
                this.enableNumberButtons(false);
                this.enableButton(true, "go");
                this.enableButton(true, "clear");
            }

            if (digit === 'clear') {

                // Enter ac numbeer

                self.display(hlp.text("pinpad_placeholder_text2"));

                //
                elemForErrcode.style.display = "block";
                elemForErrcode.className = "";

                this.enableNumberButtons(true);
                this.enableButton(false, "go");
                this.enableButton(false, "clear");
                this.accessNumber = "";


                // Clear the ac num

                if (iserror) {
                    accNumHolder.innerHTML = "";
                    this.enableNumberButtons(true);

                } else {
                    this.accessNumber = "";
                    accNumHolder.innerHTML = "";
                    this.enableNumberButtons(true);
                }

            }

    };

    /**
     *  wrap all buttons function inside ...
     *
     * @param {type} enable
     * @param {type} buttonName
     * @returns {undefined}
     */
    mpin.prototype.enableButton = function(enable, buttonName) {

        var buttonValue = {}, _element;
        buttonValue.go = {id: "mpinLogin", trueClass: "btnLogin", falseClass: "btnLogin disabled"};
        buttonValue.clear = {id: "mpinClear", trueClass: "btnClear", falseClass: "btnClear disabled"};
        buttonValue.toggle = {id: "mp_toggleButton", trueClass: "mp_DisabledState", falseClass: ""};
        _element = document.getElementById(buttonValue[buttonName].id);
        if (!buttonValue[buttonName] || !_element) {
            return;
        }

        _element.disabled = !enable;
        _element.className = buttonValue[buttonName][enable + "Class"];
    };
    //showInPinPadDisplay
    mpin.prototype.display = function(message, clear) {

        var elemForErrcode = document.getElementById('codes');
        elemForErrcode.style.display = "block";
        elemForErrcode.className = "error";
        elemForErrcode.innerHTML = message;

    };

    mpin.prototype.resetDisplay = function(message) {

        var elemForErrcode = document.getElementById('codes');
        elemForErrcode.style.display = "none";
        elemForErrcode.innerHTML = message;

    };

    mpin.prototype.bindCircles = function() {

        var pinElement = document.getElementById('pinpad-input');
        var newCircle = document.createElement('div');
        newCircle.className = "inner-circle";
        var circleID = "mpin_circle_" + (this.pinpadInput.length - 1);
        document.getElementById(circleID).appendChild(newCircle);
    }

    mpin.prototype.clearCircles = function() {

        var pinSize = mpin.cfg.pinSize, circles = [];
        for (var i = 0; i <= pinSize; i++) {
            circles[i] = document.getElementById("mpin_circle_" + i);
            if (circles[i] && circles[i].childNodes[3]) {
                circles[i].removeChild(circles[i].childNodes[3]);
            }
        }
    }

    mpin.prototype.getDisplayName = function(uId) {
        if (!uId)
            uId = this.identity;
        try {
            return JSON.parse(mp_fromHex(uId)).userID;
        } catch (err) {
            return uId;
        }
    };

    mpin.prototype.toggleButton = function() {
        var self = this;
        var accountTopBar = document.getElementById('accountTopBar')
        var menuBtn = document.getElementById("menuBtn");

        if (menuBtn && !menuBtn.classList.contains("close")) {

            // this.setIdentity(this.identity, true, function() {
            // }, function() {
            //     return false;
            // });

            accountTopBar.style.height = "100%"
            menuBtn.className = 'close';

            removeClass("mp_toggleButton", "mp_SelectedState");
            removeClass("mp_panel", "mp_flip");

            this.renderAccountsPanel();


        } else {

            if (this.ds.getIdentityToken(this.identity) == "") {
                        identity = this.getDisplayName(this.identity);
                        this.renderIdentityNotActive(identity);
                        return;
                    }

        }

        return false;
    };


    mpin.prototype.renderAccountsBeforeSetup = function() {
        var self = this;
        var accountTopBar = document.getElementById('identityContainer')


        this.setIdentity(this.identity, true, function() {
        }, function() {
            return false;
        });

        accountTopBar.style.height = "100%"

        removeClass("mp_toggleButton", "mp_SelectedState");
        removeClass("mp_panel", "mp_flip");

        this.renderAccountsBeforeSetupPanel();

        return false;
    };

    mpin.prototype.actionSetupHome = function(uId) {

        var _email, _deviceName, _deviceNameInput, _reqData = {}, self = this, elems = [], removeError;

        _email = (uId) ? uId : document.getElementById("emailInput").value.toLowerCase();

        if (_email.length === 0 || !this.opts.emailCheckRegex.test(_email)) {
            document.getElementById("emailInput").focus();
			
			elems[0] = document.getElementsByClassName("inputLabel")[0];
            elems[1] = document.getElementsByClassName("userLabel")[0];
            elems[2] = document.getElementById("emailInput");
			
			removeError = function (event) {
              elems[0].className = "inputLabel";
              elems[1].className = "userLabel";
              elems[2].className = "";
              
              elems[2].removeEventListener("touchstart", function () {});
            };
			
			
			elems[0].className += " inputLabelErr";
			elems[1].className += " userLabelErr";
			elems[2].className = "emailInputErr";
			
			elems[2].removeEventListener("touchstart", function () {});
			elems[2].addEventListener("touchstart", removeError);
			
            return;
        }

        _reqData.URL = this.opts.registerURL;
        _reqData.method = "PUT";
        _reqData.data = {
            userId: _email,
            mobile: 1
        };

        _deviceNameInput = (document.getElementById("deviceInput")) ? document.getElementById("deviceInput").value.trim() : "";
        //DEVICE NAME
        if (!this.ds.getDeviceName() && _deviceNameInput === "") {
            _deviceName = this.suggestDeviceName();
        } else if (!this.ds.getDeviceName() && _deviceNameInput !== "") {
            _deviceName = _deviceNameInput;
        } else if (_deviceNameInput !== this.ds.getDeviceName()) {
            _deviceName = _deviceNameInput;
        } else {
            _deviceName = false;
        }

        if (_deviceName) {
            this.ds.setDeviceName(_deviceName);
        }
		if (this.opts.setDeviceName) {
			_reqData.data.deviceName = (_deviceNameInput === "") ? this.suggestDeviceName() : _deviceNameInput;
		}

        //register identity
        requestRPS(_reqData, function(rpsData) {
            if (rpsData.error) {
                self.error("Activate First");
                return;
            }
            self.ds.addIdentity(rpsData.mpinId, "");
            self.ds.setIdentityData(rpsData.mpinId, {regOTT: rpsData.regOTT});

            self.ds.setDefaultIdentity(rpsData.mpinId);
            self.identity = rpsData.mpinId;

            // Check for existing userid and delete the old one
            self.ds.deleteOldIdentity(rpsData.mpinId);

            //active = true pass activate IDNETITY Screen
             if (rpsData.active) {
              self.beforeRenderSetup();
             } else {
              self.renderActivateIdentity();
             }
        });
    };

    mpin.prototype.requestSignature = function(email, clientSecretShare, clientSecretParams) {
        var self = this;

        requestClientSecret(self.certivoxClientSecretURL(clientSecretParams), clientSecretShare, function(clientSecret) {

            self.enableNumberButtons(true);
            self.clientSecret = clientSecret;

            if (self.opts.onGetSecret) {
                self.opts.onGetSecret();
            }
        }, function(message, code) {
            self.error(message, code);
        });

    };

    mpin.prototype.error = function(msg) {
        if (this.opts.onError) {
            this.opts.onError(msg);
        } else {
            console.error("Error : " + msg);
        }
    };

    mpin.prototype.actionResend = function(btnElem) {
        var self = this, _reqData = {}, regOTT, _email, btn;

        regOTT = this.ds.getIdentityData(this.identity, "regOTT");
        _email = this.getDisplayName(this.identity);

        btn = this.mpinButton(btnElem, "setupNotReady_resend_info1");

        _reqData.URL = this.opts.registerURL;
        _reqData.URL += "/" + this.identity;
        _reqData.method = "PUT";
        _reqData.data = {
            userId: _email,
            mobile: 1,
            regOTT: regOTT
        };

        if (this.opts.registerRequestFormatter) {
            _reqData.postDataFormatter = this.opts.registerRequestFormatter;
        }
        if (this.opts.customHeaders) {
            _reqData.customHeaders = this.opts.customHeaders;
        }

        // add identity into URL + regOTT
        requestRPS(_reqData, function(rpsData) {
            if (rpsData.error || rpsData.errorStatus) {
                self.error("Resend problem");
                return;
            }

            if (self.identity !== rpsData.mpinId) {
                //delete OLD mpinID
                self.ds.deleteIdentity(self.identity);

                //asign new one, create & set as default
                self.identity = rpsData.mpinId;
                self.ds.addIdentity(self.identity, "");
                self.ds.setDefaultIdentity(self.identity);
            }

            //should be already exist only update regOTT
            self.ds.setIdentityData(self.identity, {regOTT: rpsData.regOTT});

            // Check for existing userid and delete the old one
            self.ds.deleteOldIdentity(rpsData.mpinId);



            btn.ok("setupNotReady_resend_info2");
        });
    };

    mpin.prototype.actionSetup = function() {

        var self = this, _pin;
        _pin = this.pinpadInput;
        this.ds.addIdentity(this.identity, "");
        this.display(hlp.text("verify_pin"));

        extractPIN(_pin, this.clientSecret, this.identity, function(tokenHex) {

            self.ds.setIdentityToken(self.identity, tokenHex);
            self.clientSecret = "";
            self.enableNumberButtons(false);
            self.enableButton(false, "go");
            self.ds.setDefaultIdentity(self.identity);
            self.ds.deleteOldIdentity(self.identity);
            self.display(hlp.text("setupPin_pleasewait"), false);

            if (self.opts.setupDoneURL) {
                var _reqData = {}, url = self.opts.setupDoneURL + "/" + self.identity;

                _reqData.URL = url;
                _reqData.method = "POST";
                _reqData.data = {};


                //get signature
                requestRPS(_reqData, function(rpsData) {

                    if (rpsData.errorStatus) {
                        self.error("ooops");
                        return;
                    }
                    self.successSetup();
                });
            } else {
                self.successSetup();
            }
        });
    };
    /**
     *
     * @returns {undefined}
     */
    mpin.prototype.actionLogin = function() {

        var callbacks = {}
            ,authServer, getAuth
            , self = this
            , pinValue = this.pinpadInput
            , accessNumber;

        //AlertMessage.clearDisplayWrap();
        this.enableNumberButtons(false);
        this.enableButton(false, "go");
        this.enableButton(false, "clear");
        this.enableButton(true, "toggle");

        this.display(hlp.text("authPin_pleasewait"));

        //getAuth = this.opts.useWebSocket ? getAuthToken : getAuthTokenAjax;
        //authServer = this.opts.mpinAuthServerURL;

        if (this.opts.useWebSocket) {
            getAuth = getAuthToken;
            authServer = this.opts.mpinAuthServerURL + "/authenticationToken";
        } else {
            getAuth = getAuthTokenAjax;
            authServer = this.opts.mpinAuthServerURL;
        }

        accessNumber = this.accessNumber;

        //authServer = this.opts.authenticateURL;
        getAuth(authServer, this.opts.appID, this.identity, this.ds.getIdentityPermit(this.identity), this.ds.getIdentityToken(this.identity), this.currentDate,
                this.opts.requestOTP, accessNumber ? accessNumber : "0", pinValue, this.opts.requestOTP ? this.opts.authenticateURL : this.opts.mobileAuthenticateURL, this.opts.authenticateRequestFormatter, this.opts.customHeaders, function(success, errorCode, errorMessage, authData) {

                    if (success) {

                        var iD = self.identity;
                            if (self.opts.requestOTP) {
                                self.renderOtp(authData);
                                return;
                            }

                            self.successLogin(authData, iD);

                    } else if (errorCode === "INVALID") {

                        self.addToPin.call(self, "clear");
                        self.display(hlp.text("authPin_errorInvalidPin"), true);

                        self.enableNumberButtons(true);

                        self.enableButton(false, "go");
                        self.enableButton(false, "clear");
                        self.enableButton(true, "toggle");

                    } else if (errorCode === "MAXATTEMPTS") {

                        var iD = self.identity;
                        self.deleteIdentity(iD);
                        if (self.opts.onAccountDisabled) {
                            self.opts.onAccountDisabled(iD);
                        }

                        callbacks.mp_action_register = function(evt) {
							var email = self.getDisplayName(iD);
                            self.renderSetupHome.call(self, email);
                        };

                        callbacks.mp_action_home = function(evt) {
                            self.renderHomeMobile.call(self);
                        };

                        // TODO: Register again or select new identity

                        self.render('access-denied', callbacks, {email: self.getDisplayName(iD)});

                    } else if (errorCode === "INVALIDACCESSNUMBER") {

                        // Render the access number again
                        self.renderAccessNumber.call(self);
                        self.addToAcc.call(self, "clear", false);
                        self.display(hlp.text("authPin_errorInvalidAccessNumber"), true);

                    } else if (errorCode === "NOTAUTHORIZED") {

                        self.display(hlp.text("authPin_errorNotAuthorized"), true);

                    } else if (errorCode === "EXPIRED") {

                        self.display(hlp.text("authPin_errorExpired"), true);

                    } else if (errorCode === "WEBSOCKETERROR") {

                        console.error("WebSocket connection fail! Falling to AJAX");
                        self.opts.useWebSocket = false;
                        self.actionLogin.call(self);

                    } else {

                        console.error("Authentication error: ", errorCode, errorMessage)
                        self.display(hlp.text("authPin_errorServer"), true);
                    }

                }, function() {
                    console.log(" Before HandleToken ::::");
        });

    };

    mpin.prototype.setIdentity = function(newIdentity, requestPermit, onSuccess, onFail) {
        var displayName, accId, self = this;

        if ((typeof(newIdentity) === "undefined") || (!newIdentity))
            displayName = "";
        else {
            this.identity = newIdentity;
            displayName = this.getDisplayName(this.identity);
        }

        accId = document.getElementById('mpinUser');

        if(accId) {
            accId.children[0].innerText = displayName;
            accId.setAttribute("title", displayName);
        }

        // no Identity go to setup HOME
        if (!this.identity) {
            this.renderSetupHome();
            return;
        }

        if (requestPermit) {

            if (this.ds.getIdentityToken(this.identity) == "") {
                this.renderIdentityNotActive(displayName);
                return;
            }

            this.enableNumberButtons(false);
            this.enableButton(false, "go");
            this.enableButton(false, "clear");
            this.enableButton(true, "toggle");
            this.requestPermit(newIdentity, function(timePermitHex) {
                self.enableNumberButtons(true);
            }, function(message, statusCode) {
                if (statusCode === 404) {
                    self.renderIdentityNotActive(displayName);
                    onFail();
                } else {
                    // Fatal server error!
                    self.display(hlp.text("pinpad_errorTimePermit") + " " + statusCode, true);
                    self.error("Error getting the time permit.", statusCode);
                    onFail();
                }
            });
        }
    };

    mpin.prototype.successSetup = function(authData) {
        var self = this;
        if (this.opts.successSetupURL) {
            window.location = this.opts.successSetupURL;
        } else if (this.opts.onSuccessSetup) {
            this.opts.onSuccessSetup(authData, function() {
                self.renderSetupDone.call(self);
            });
        } else {
            this.renderSetupDone();
        }
    };

    //Get request
    mpin.prototype.ajax = function(url, cb) {
        var _request = new XMLHttpRequest();
        _request.open("GET", url, true);
        _request.send();

        _request.onreadystatechange = function() {
            if (_request.readyState === 4 && _request.status === 200)
            {
                cb(JSON.parse(_request.responseText));
            } else if (_request.readyState === 4 && !navigator.onLine) {
                cb({error: 500});
            }
        };

    };

    //Post request
    mpin.prototype.ajaxPost = function(url, data, cb) {
        var _request = new XMLHttpRequest();
        _request.onreadystatechange = function() {
            if (_request.readyState === 4 && _request.status === 200)
            {
                // Tempory fix
                if (_request.responseText == '') {
                    cb(true);
                }
            } else if (_request.readyState === 4) {
                    cb(false);
			}
        };
        _request.open("Post", url, true);
        _request.send(JSON.stringify(data));
    };

    //new Function
    mpin.prototype.requestPermit = function(identity, onSuccess, onFail) {
        var self = this;
        requestTimePermit(self.certivoxPermitsURL(), self.dtaPermitsURL(), self.opts.authenticateHeaders,
                self.ds.getIdentityPermitCache(this.identity), this.certivoxPermitsStorageURL(),
                function(timePermitHex, timePermitCache, currentDate) {
                    self.ds.setIdentityPermit(self.identity, timePermitHex);
                    self.ds.setIdentityPermitCache(mpin.identity, timePermitCache);
                    self.currentDate = currentDate || util.today();
                    self.ds.save();
                    self.gotPermit(timePermitHex);
                    onSuccess(timePermitHex);
                },
                function(message, statusCode) {
                    onFail(message, statusCode)
                });

    };

    mpin.prototype.deleteIdentity = function(iID) {
        var newDefaultAccount = "", self = this;

        this.ds.deleteIdentity(iID);
        for (var i in this.ds.getAccounts()) {
            newDefaultAccount = i;
            break;
        }

        if (newDefaultAccount) {
            this.setIdentity(newDefaultAccount, true, function() {
            }, function() {
                return false;
            });

            this.ds.setDefaultIdentity(newDefaultAccount);
            // Render the identity list panel
            this.renderAccountsPanel();
        } else {
            this.setIdentity(newDefaultAccount, false);
            this.ds.setDefaultIdentity("");
            this.identity = "";
            this.renderSetupHome();
        }
        return false;
    };

    //data Source with static referance
    mpin.prototype.dataSource = function() {
        var mpinDs = {}, self = this;
        this.ds || (this.ds = {});
        if (typeof(localStorage['mpin']) === "undefined") {
            localStorage.setItem("mpin", JSON.stringify({
                defaultIdentity: "",
                version: "0.3",
                accounts: {}
            }));
        }
        mpinDs.mpin = JSON.parse(localStorage.getItem("mpin"));

        mpinDs.addIdentity = function(uId, token, permit) {
            if (!mpinDs.mpin.accounts[uId]) {
                mpinDs.mpin.accounts[uId] = {"MPinPermit": "", "token": ""};
            }
            //this.mpin.defaultIdentity = uId;
            mpinDs.setIdentityToken(uId, token);
            if (permit)
                mpinDs.setIdentityPermit(uId, permit);
        };
        mpinDs.setIdentityToken = function(uId, value) {
            mpinDs.mpin.accounts[uId]["token"] = value;
            mpinDs.save();
        };
        mpinDs.setIdentityPermit = function(uId, value) {
            mpinDs.mpin.accounts[uId]["MPinPermit"] = value;
            mpinDs.save();
        };
        mpinDs.getIdentityPermit = function(uId) {
            if (!uId)
                uId = mpinDs.getDefaultIdentity();
            return mpinDs.mpin.accounts[uId]["MPinPermit"];
        };
        mpinDs.setIdentityPermitCache = function(uId, cache) {
            if (!uId) {
                uId = mpinDs.getDefaultIdentity();
            }
            mpinDs.mpin.accounts[uId]["timePermitCache"] = cache;
            mpinDs.save();
        };
        mpinDs.getIdentityPermitCache = function(uId) {
            if (!uId) {
                uId = mpinDs.getDefaultIdentity();
            }
            return mpinDs.mpin.accounts[uId]["timePermitCache"] || {};
        };
        mpinDs.getIdentityToken = function(uId) {
            if (!uId)
                uId = mpinDs.getDefaultIdentity();
            return mpinDs.mpin.accounts[uId]["token"];
        };
        mpinDs.getDefaultIdentity = function(uId) {
            return mpinDs.mpin.defaultIdentity;
        };
        mpinDs.setDefaultIdentity = function(uId) {
            mpinDs.mpin.defaultIdentity = uId;
            mpinDs.save();
        };
        mpinDs.deleteOldIdentity = function(uId) {
            var name = self.getDisplayName(uId);

            for (var i in this.getAccounts()) {
                if (i !== uId) {
                    var oName = self.getDisplayName(i);
                    if (oName === name) {
                        mpinDs.deleteIdentity(i);
                    }
                }
            }
        };
        mpinDs.deleteIdentity = function(uId) {
            delete mpinDs.mpin.accounts[uId];
            mpinDs.save();
        };
        mpinDs.save = function() {
            localStorage.setItem("mpin", JSON.stringify(mpinDs.mpin));
        };
        mpinDs.getAccounts = function() {
            return mpinDs.mpin.accounts;
        };

        mpinDs.setIdentityData = function(uId, values) {
            for (var v in values) {
                mpinDs.mpin.accounts[uId][v] = values[v];

            }
            mpinDs.save();
        };

        mpinDs.setDeviceName = function(devId) {
            mpinDs.mpin.deviceName = devId;
            mpinDs.save();
            };

        mpinDs.getDeviceName = function() {
            var deviceID;
            deviceID = mpinDs.mpin.deviceName;
            if (!deviceID) {
                return false;
            }

            return deviceID;
        };

        mpinDs.getIdentityData = function(uId, key) {
            return mpinDs.mpin.accounts[uId][key];
        };

        return mpinDs;
    };

    mpin.prototype.successLogin = function(authData, iD) {
        var callbacks = {},
            self = this,
			totalAccounts;

		totalAccounts = self.ds.getAccounts();
        totalAccounts = Object.keys(totalAccounts).length;

        callbacks.mp_action_home = function(evt) {
            if (totalAccounts === 0) {
             self.renderSetupHome();
            } else if (totalAccounts === 1) {
             self.renderAccessNumber();
            } else if (totalAccounts > 1) {
             self.renderAccessNumber(true);
            }
        };

        callbacks.mp_action_logout = function(evt) {
            var that = this;
            if(authData.logoutURL) {
                self.ajaxPost( authData.logoutURL, authData.logoutData, function(res) {
                    if (res) {
                        // self.renderAccessNumber();
                        self.renderAccessNumber.call(self);
                    } else {
						that.innerHTML = '<span class="btnLabel" id="btnLabelText" style="color: red;">Sign out was unsuccessful !</span>';
						setTimeout(function () {
							self.renderAccessNumber.call(self);
						}, 2000);
					}

                });
            } else {

                // self.renderAccessNumber();
                self.renderAccessNumber.call(self);
            }

        };

        this.render("success-login", callbacks, {email: self.getDisplayName(iD)});

        if(authData.logoutURL === '') {
            var _logoutBtnText = document.getElementById('btnLabelText');
            _logoutBtnText.innerText = hlp.text("start_over");
        }
    };

    mpin.prototype.certivoxClientSecretURL = function(params) {
        return this.opts.certivoxURL + "clientSecret?" + params;
    };

     mpin.prototype.certivoxPermitsURL = function() {
		var mpin_id_bytes, hash_mpin_id_bytes = [], hash_mpin_id_hex,
				permitsUrl, mpData = mp_fromHex(this.identity);

		mpin_id_bytes = MPIN.stringtobytes(mpData);
		hash_mpin_id_bytes = MPIN.HASH_ID(mpin_id_bytes);
		hash_mpin_id_hex = MPIN.bytestostring(hash_mpin_id_bytes);

		permitsUrl = this.opts.certivoxURL + "timePermit";
		permitsUrl += "?app_id=" + this.opts.appID;
		permitsUrl += "&mobile=0";
		permitsUrl += "&hash_mpin_id=" + hash_mpin_id_hex;
		return permitsUrl;
    };

    mpin.prototype.dtaPermitsURL = function() {
        var mpin_idHex = this.identity;
        return this.opts.timePermitsURL + "/" + mpin_idHex;
    };

    mpin.prototype.certivoxPermitsStorageURL = function() {
        var that = this;

        return function(date, storageId) {
            console.log("timePermitsStorageURL Base: " + that.opts.timePermitsStorageURL)
            if ((date) && (that.opts.timePermitsStorageURL) && (storageId)) {
                return that.opts.timePermitsStorageURL + "/" + that.opts.appID + "/" + date + "/" + storageId;
            } else {
                return null;
            }
        }
    };


    mpin.prototype.gotPermit = function(timePermit) {
        if (this.opts.onGetPermit)
            this.opts.onGetPermit(timePermit);
    };

    mpin.prototype.checkAccessNumberValidity = function(sNum, csDigits){
        if (!csDigits) {
            csDigits = 1;
        }

        var n = parseInt(sNum.slice(0, sNum.length-csDigits), 10);
        var cSum = parseInt(sNum.slice(sNum.length-csDigits, sNum.length), 10);

        var p = 99991;
        var g = 11;
        var checkSum = ((n * g) % p) % Math.pow(10, csDigits);

        return (checkSum == cSum)
    };

	// Better checksum approach
	// checkAccessNumberValidity2 (accessNumber, Length)
	mpin.prototype.checkAccessNumberValidity2 = function (number, len) {
		var cSum, checksum, x, w, wid, wid_len, g = 11, sum_d = 0;
		wid = number.toString();
		wid = wid.substring(0, number.toString().length-1);
		w = len + 1;
		sum_d = 0;
		wid_len = wid.length;

		for (var i = 0 ; i < wid_len; i ++) {
			x = parseInt(wid[i]);
			sum_d += (x*w);
			w -= 1;
		}
		checksum = (g - (sum_d%g)) % g;
		checksum = (checksum === 10) ? 0 : checksum;

		//get last one digit and compare with checksum result
		cSum = number.substr(-1);
		cSum = parseInt(cSum);
		return (cSum === checksum);
	}

    function mp_fromHex(s) {
        if (!s || s.length % 2 != 0)
            return '';

        s = s.toLowerCase();
        var digits = '0123456789abcdef';
        var result = '';
        for (var i = 0; i < s.length; ) {
            var a = digits.indexOf(s.charAt(i++));
            var b = digits.indexOf(s.charAt(i++));
            if (a < 0 || b < 0)
                return '';
            result += String.fromCharCode(a * 16 + b);
        }
        return result;
    }
    ;

    // HELPERS and Language Dictionary

    loader = function(url, callback) {
        var script = document.createElement('script');
        script.type = 'text/javascript';
        script.src = url;
        //IE feature detect
        if (script.readyState) {
            script.onreadystatechange = callback;
        } else {
            script.onload = callback;
        }
        document.getElementsByTagName('head')[0].appendChild(script);
    };

    function addClass(elId, className) {
        var el;
        if (typeof(elId) === "string") {
            el = document.getElementById(elId);
        } else {
            el = elId;
        }

        if (el.className) {
            var cNames = el.className.split(/\s+/g);
            if (cNames.indexOf(className) < 0)
                el.className += " " + className;
        } else {
            el.className = className;
        }
    }
    ;

    function hasClass(elId, className) {
        var el;
        if (typeof(elId) == "string")
            el = document.getElementById(elId);
        else
            el = elId;

        var cNames = el.className.split(/\s+/g);
        return (cNames.indexOf(className) >= 0)
    }
    ;

    function removeClass(elId, className) {
        var el;
        if (typeof(elId) == "string")
            el = document.getElementById(elId);
        else
            el = elId;

        if ((el) && (el.className)) {
            var cNames = el.className.split(/\s+/g);
            cNames.splice(cNames.indexOf(className), 1);
            el.className = cNames.join(" ");
        }
    }
    ;

    //private variable
    //en
    mpin.lang = {};
    mpin.lang.en = {
        "pinpad_initializing": "Initializing...",
        "pinpad_errorTimePermit": "ERROR GETTING PERMIT:",
        "home_alt_mobileOptions": "Mobile Options",
        "home_button_authenticateMobile_noTrust": "Sign in with Smartphone <br> (This is a PUBLIC device which I DO NOT trust)",
        "home_button_authenticateMobile_trust": "Sign in with Browser <br> (This is a PERSONAL device which I DO trust)",
        "home_button_authenticateMobile_intro": "First let's establish trust to choose the best way for you to access this service:",
        "home_button_authenticateMobile_description": "Get your Mobile Access Number to use with your M-Pin Mobile App to securely authenticate yourself to this service.",
        "home_button_getMobile": "Get <br/>M-Pin Mobile App",
        "home_button_getMobile_description": "Install the free M-Pin Mobile App on your Smartphone now!  This will enable you to securely authenticate yourself to this service.",
        "home_button_authenticateBrowser": "Authenticate <br/>with this Browser",
        "home_button_authenticateBrowser_description": "Enter your M-PIN to securely authenticate yourself to this service.",
        "home_button_setupBrowser": "Add an <br/>Identity to this Browser",
        "home_button_setupBrowser_description": "Add your Identity to this web browser to securely authenticate yourself to this service using this machine.",
        "mobileGet_header": "GET M-PIN MOBILE APP",
        "mobileGet_text1": "Scan this QR code",
        "mobileGet_text2": "or open this URL on your mobile:",
        "mobileGet_button_back": "Back",
        "mobileAuth_header": "AUTHENTICATE WITH YOUR M-PIN",
        "mobileAuth_seconds": "seconds",
        "mobileAuth_text1": "Your Access Number is:",
        "mobileAuth_text2": "Note: Use this number in the next",
        "mobileAuth_text3": "with your M-Pin Mobile App.",
        "mobileAuth_text4": "Warning: Navigating away from this page will interrupt the authentication process and you will need to start again to authenticate successfully.",
        "otp_signin_header": "Sign in with One-Time Password",
        "otp_text1": "Your One-Time Password is:",
        "otp_text2": "Note: The password is only valid for " + mpin.cfg.expireOtpSeconds + " seconds before it expires.", // {0} will be replaced with the max. seconds
        "otp_seconds": "Remaining:", // {0} will be replaced with the remaining seconds
        "otp_expired_header": "Your One-Time Password has expired.",
        "otp_expired_button_home": "Login again to get a new OTP",
        "setup_header": "Add an identity",
        "setup_text1": "Email address:",
        "setup_text2": "Your email address will be used as your identity when M-Pin authenticates you to this service.",
        "setup_text3": "Enter your email",
        "setup_error_unathorized": "{0} has not been registered in the system.", // {0} will be replaced with the userID
        "setup_error_server": "Cannot process the request. Please try again later.",
        "setup_error_signupexpired": "Your signup request has been expired. Please try again.",
        "setup_button_setup": "Setup M-Pin",
        "setupPin_header": "Create your M-Pin with {0} digits", // {0} will be replaced with the pin length
        "setupPin_initializing": "Initializing...",
        "setupPin_pleasewait": "Please wait...",
        "setupPin_button_clear": "Clear",
        "setupPin_button_done": "Setup Pin",
        "setupPin_errorSetupPin": "ERROR SETTING PIN: {0}", // {0} is the request status code
        "setupDone_header": "Congratulations!",
        "edit_identity": "Edit Identity:",
        "setupDone_text1": "Your M-Pin Identity:",
        "setupDone_text2": "is set up successfully.",
        "setupDone_text3": "",
        "setupDone_button_go": "Sign in with this M-Pin",
        "setupReady_header": "VERIFY YOUR IDENTITY",
        "setupReady_text1": "We have sent you an email to",
        "setupReady_text2": "Click the link in the email to confirm your identity and proceed",
        "setupReady_text3": "We have just sent you an email, simply click the link to verify your identity.",
        "setupReady_button_go": "I confirmed my email",
        "setupReady_button_go_cont": "Setup your M-Pin now",
        "setupReady_button_resend": "Resend confirmation email",
        "setupReady_button_resend_cont": "Send it again",
        "setupNotReady_header": "YOU MUST VERIFY <br/>YOUR IDENTITY",
        "setupNotReady_text1": "Your identity",
        "setupNotReady_text2": "has not been verified.",
        "setupNotReady_text3": "You need to click the link in the email we sent you, and then choose <br/> 'Setup M-Pin'.",
        "setupNotReady_check_info1": "Checking",
        "setupNotReady_check_info2": "Identity not verified!",
        "setupNotReady_resend_info1": "Sending email",
        "setupNotReady_resend_info2": "Email sent!",
        "setupNotReady_button_check": "Setup M-Pin",
        "setupNotReady_button_resend": "Send the email again",
        "setupNotReady_button_back": "Go to the identities list",
        "authPin_header": "Enter your M-Pin",
        "authPin_button_clear": "Clear",
        "authPin_button_login": "Login",
        "authPin_button_next": "Next",
        "authPin_pleasewait": "Authenticating...",
        "authPin_success": "Success!",
        "authPin_errorInvalidPin": "Incorrect Pin!",
        "authPin_errorInvalidAccessNumber": "Invalid access number!",
        "authPin_errorNotAuthorized": "You are not authorized!",
        "authPin_errorExpired": "The auth request expired!",
        "authPin_errorServer": "Server error!",
        "deactivated_header": "SECURITY ALERT",
        "deactivated_text1": "has been de-activated and your M-Pin token has been revoked.",
        "deactivated_text2": "To re-activate your identity, click on the blue button below to register again.",
        "deactivated_button_register": "Register again",
        "account_button_addnew": "Add a new identity to this list",
        "account_button_delete": "Remove Identity",
        "account_button_reactivate": "Reset PIN",
        "account_button_backToList": "Back to identity list",
        "account_button_cancel": "Cancel and go back",
        "account_delete_question": "Are you sure you wish to remove this M-Pin Identity from this browser?",
        "account_delete_button": "Yes, remove it",
        "account_reactivate_question": "Are you sure you wish to reactivate this M-Pin Identity?",
        "account_reactivate_button": "Yes, re-activate",
        "noaccount_header": "No identities have been added to this browser!",
        "noaccount_button_add": "Add a new identity",
        "pinpad_placeholder_text": "Enter your PIN",
        "pinpad_placeholder_setup": "Setup your PIN",
        "pinpad_placeholder_text2": "Enter access number",
        "logout_text1": "YOU ARE NOW LOGGED IN",
        "logout_button": "Logout",
        "home_button_setupMobile": "Add an identity to this browser",
        "mobile_splash_text": "INSTALL THE M-PIN MOBILE APP",
        "mobile_add_home_ios": "Tap the icon to 'Add to homescreen'",
        "help_text_1": "Simply choose a memorable <b>[4 digit]</b> PIN to assign to this identity by pressing the numbers in sequence followed by the 'Setup' button to setup your PIN for this identity",
        "help_ok_btn": "Ok, Got it",
        "help_more_btn": "I'm not sure, tell me more",
        "logout_btn": "Sign out",
        "success_header": "Success",
        "success_text": "You are now signed in as:",
        "accessdenied_header": "Access Denied",
        "accessdenied_text": "Your M-Pin identity",
        "accessdenied_text_cont": "has been removed from this device.",
        "accessdenied_btn": "Register again",
        "setup_btn_text": "Setup",
        "setup_device_label": "Device name:",
        "verify_pin": "Verifying PIN...",
        "sign_in": "Sign In",
        "clear": "Clear",
        "setup": "Setup",
        "start_over": "Start Over",
        "embedded_header": "To function properly, M-Pin should be opened in Safari. To open it in Safari, do one  of the following:",
        "embedded_p1": "1. If your QR Code Reader provides the capability to open the page in Safari, please use this option to do so.",
        "embedded_p2": "2. Copy the URL from the address bar, open Safari, Paste the URL in its address bar and proceed according the on-screen instructions.",
    };
    //  image should have config properties
    hlp.img = function(imgSrc) {
        return IMAGES_PATH + imgSrc;
    };
    //  translate
    hlp.text = function(langKey) {
        return mpin.lang[hlp.language][langKey];
    };

    var setStringOptions = function() {
        if (typeof(String.prototype.trim) === "undefined")
        {
            String.prototype.mpin_trim = function() {
                return String(this).replace(/^\s+|\s+$/g, '');
            };
        } else {
            String.prototype.mpin_trim = String.prototype.trim;
        }

        String.prototype.mpin_endsWith = function(substr) {
            return this.length >= substr.length && this.substr(this.length - substr.length) == substr;
        }

        String.prototype.mpin_startsWith = function (substr) {
         return this.indexOf(substr) == 0;
        }

        if (!String.prototype.mpin_format) {
            String.prototype.mpin_format = function() {
                var args = arguments;
                return this.replace(/{(\d+)}/g, function(match, number) {
                    return typeof args[number] != 'undefined'
                            ? args[number]
                            : match
                            ;
                });
            };
        }
    };
})();
