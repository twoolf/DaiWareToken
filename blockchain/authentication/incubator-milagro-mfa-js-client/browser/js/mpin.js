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

(function () {
  "use strict";
  var lang = {}, hlp = {}, loader, MPIN_URL_BASE, IMAGES_PATH, CSS_FILENAME;
  MPIN_URL_BASE = "%URL_BASE%";
  IMAGES_PATH = MPIN_URL_BASE + "/images/";

  CSS_FILENAME = "main.css";

  //CONSTRUCTOR
  mpin = function (options) {
    var self = this, domID;

    loader(MPIN_URL_BASE + "/css/" + CSS_FILENAME, "css", function () {
      var opts = {}, mpinServer, docLoc;

      Handlebars.registerHelper("txt", function (optionalValue) {
        return hlp.text(optionalValue);
      });

      Handlebars.registerHelper("img", function (optionalValue) {
        return hlp.img(optionalValue);
      });

      Handlebars.registerHelper("loop", function (n, block) {
        var accum = '';
        for (var i = 0; i < n; ++i)
          accum += block.fn(i);
        return accum;
      });
      Handlebars.registerHelper("substr", function (longStr) {
        if (longStr.length < 82) {
          return longStr;
        } else {
          return longStr.substr(0, 82) + "...";
        }
      });


      if (!options.clientSettingsURL) {
        return self.error(4002);
      }

      self.currentDate = null;

      //http || https
      if (options.clientSettingsURL.indexOf("http") === -1) {
        docLoc = document.location;
        mpinServer = docLoc.protocol + "//" + docLoc.host;
      } else {
        var tmpSrv = options.clientSettingsURL.split("/");
        mpinServer = tmpSrv[0] + "//" + tmpSrv[2];
      }

      self.mpinLib = new mpinjs({
        server: mpinServer,
        authProtocols: options.authProtocols || ""
      });

      domID = options.targetElement;
      opts.client = options;

      self.mpinLib.init(function (serverOptions) {
        if (options || options.targetElement) {
          self.el = document.getElementById(options.targetElement);
          addClass(self.el, "mpinMaster");
          self.setupHtml();
        } else {
          return console.error("::: TargetElement are missing or wrong !");
        }
        self.recover();

        if (serverOptions && serverOptions.error) {
          return self.error(serverOptions.error);
        }
        opts.server = serverOptions;

        // check if Dom ready if not wait until fire load event.
        if (document.readyState === "complete") {
          self.initialize.call(self, domID, opts);
        } else {
          window.addEventListener("load", function () {
            self.initialize.call(self, domID, opts);
          });
        }
      });

    });
  };

  //CONFIGS
  mpin.prototype.cfg = {
    language: "en",
    pinSize: 4,
    requiredOptions: "appID; seedValue; signatureURL; mpinAuthServerURL; timePermitsURL; authenticateURL; registerURL",
    restrictedOptions: "signatureURL; mpinAuthServerURL; timePermitsURL",
    renderMap: {"add": "renderAddIdentity", "home": "renderHome"},
    defaultOptions: {
      identityCheckRegex: /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/,
      setDeviceName: false,
      mobileSupport: true,
      mobileOnly: false,
      keyboardEnable: true,
      mobileNativeApp: false
    },
    expireOtpSeconds: 99,
    lastRequestDelay: 3000 //milliseconds
  };

  /**
   * Mpin Constructor
   *
   * @param {type} domID PinPad element ID
   * @param {type} options
   *
   * @returns {Boolean}
   */
  mpin.prototype.initialize = function (domID, options) {
    this.addHelp();

    //set Options
    this.setDefaults().setOptions(options.client);

    //if false browser unsupport
    if (!this.checkBrowser()) {
      return;
    }

    this.opts.mobileSupport = (this.opts.mobileAppFullURL) ? this.opts.mobileSupport : false;

    //if set & exist
    if (this.opts.language && lang[this.opts.language]) {
      this.language = this.opts.language;
    } else {
      this.language = this.cfg.language;
    }

    this.setLanguageText();
    this.renderLanding();
  };

  mpin.prototype.setupHtml = function () {
    this.el.innerHTML = Handlebars.templates["mpin"]();
    this.el = document.getElementById("mpinMiracle");
  };

  mpin.prototype.checkBrowser = function () {
    var navAgent, onUnsupport = true, ieVer;
    navAgent = navigator.userAgent.toLowerCase();

    if (navAgent.indexOf('msie') !== -1) {
      ieVer = parseInt(navAgent.split('msie')[1], 10);
      if (ieVer < 10) {
        this.unsupportedBrowser(4004);
        onUnsupport = false;
      }
    }

    if (typeof window.localStorage === "undefined") {
      this.unsupportedBrowser(4005);
      onUnsupport = false;
    }

    return onUnsupport;
  };

  mpin.prototype.unsupportedBrowser = function (errCode) {
    var errMessage;
    if (this.opts.onUnsupportedBrowser) {
      errMessage = hlp.text("error_code_" + errCode);
      this.opts.onUnsupportedBrowser(errMessage);
    } else {
      this.renderError(errCode);
    }
    return;
  };

  // check minimal required Options
  //  which should be set up
  mpin.prototype.checkOptions = function (options) {
    var opts, k;
    opts = this.cfg.requiredOptions.split("; ");
    for (k = 0; k < opts.length; k++) {
      if (typeof options[opts[k]] === "undefined") {
        return false;
      }
    }
    return true;
  };
  //set defaults OPTIONS
  mpin.prototype.setDefaults = function () {
    this.opts || (this.opts = {});
    for (var i in this.cfg.defaultOptions) {
      this.opts[i] = this.cfg.defaultOptions[i];
    }

    this.opts.useWebSocket = ('WebSocket' in window && window.WebSocket.CLOSING === 2);

    return this;
  };
  mpin.prototype.setOptions = function (options) {
    var _i, _opts, _optionName, _options = "requestOTP; successSetupURL; onSuccessSetup; successLoginURL; onSuccessLogin; onLoaded; onGetPermit; ";
    _options += "onAccountDisabled; onUnsupportedBrowser; prerollid; onError; onGetSecret; signatureURL; certivoxURL; ";
    _options += "mpinAuthServerURL; registerURL; accessNumberURL; mobileAppFullURL; customHeaders; authenticateRequestFormatter; accessNumberRequestFormatter; ";
    _options += "registerRequestFormatter; identityCheckRegex; seedValue; appID; useWebSocket; setupDoneURL; timePermitsURL; timePermitsStorageURL; authenticateURL; ";
    _options += "language; customLanguageTexts; setDeviceName; getAccessNumberURL; mobileSupport; mobileOnly; keyboardEnable; landingPage; mobileNativeApp; mobileConfigURL";
    _opts = _options.split("; ");
    this.opts || (this.opts = {});
    for (_i = 0; _i < _opts.length; _i++) {
      _optionName = _opts[_i];
      if (typeof options[_optionName] !== "undefined")
        this.opts[_optionName] = options[_optionName];
    }

    return this;
  };
  mpin.prototype.addHelp = function () {
    var hlpHtml;
    hlpHtml = Handlebars.templates["help-tooltip"]();
    this.el.insertAdjacentHTML("afterend", hlpHtml);
    this.elHelpOverlay = document.getElementById("mpinHelpTag");
    this.elHelp = document.getElementById("mpinHelpContainer");
  };
  mpin.prototype.readyHtml = function (tmplName, tmplData) {
    var data = tmplData, html;
    html = Handlebars.templates[tmplName]({data: data, cfg: this.cfg});
    if (html[0] !== "<") {
      html = html.substr(1);
    }
    return html;
  };
  mpin.prototype.render = function (tmplName, callbacks, tmplData) {
    var data = tmplData || {}, k, self = this, homeElem;
    this.el.innerHTML = this.readyHtml(tmplName, data);
    for (k in callbacks) {
      if (document.getElementById(k)) {
        document.getElementById(k).onclick = callbacks[k];
      }
    }

    //mpin_home - can remove all mpin_home definition
    homeElem = document.getElementById("mpin_home");
    if (homeElem && !callbacks.mpin_home) {
      homeElem.onclick = function () {
        self.renderHome.call(self);
      };
    }
  };
  /**
   * funciton	setLanguageText
   *
   * replace lang with customLanguageTexts
   */
  mpin.prototype.setLanguageText = function () {
    hlp.language = this.language;
    //		setLanguageText
    if (this.opts.customLanguageTexts && this.opts.customLanguageTexts[this.language]) {
      for (var k in this.opts.customLanguageTexts[this.language]) {
        if (lang[this.language][k]) {
          lang[this.language][k] = this.opts.customLanguageTexts[this.language][k];
        }
      }
    }
  };
  mpin.prototype.toggleHelp = function () {
    if (this.elHelpOverlay.style.display === "block") {
      this.elHelpOverlay.style.display = "none";
      this.elHelpOverlay.style.opacity = "0";
      this.elHelp.style.display = "none";
    } else {
      this.elHelpOverlay.style.display = "block";
      this.elHelpOverlay.style.opacity = "1";
      this.elHelp.style.display = "block";
    }
  };
  //////////////////////// //////////////////////// ////////////////////////
  //////////////////////// RENDERS BEGIN FROM HERE
  //////////////////////// //////////////////////// ////////////////////////

  //landing Page
  mpin.prototype.renderLanding = function () {
    var self = this, totalAccounts, userId;

    this.clrInterval();


    //check for prerollid
    if (this.opts.prerollid) {
      userId = this.readIdentity();
      //check if this identity is not register already !!!
      if (userId !== this.opts.prerollid) {

        var idenList = this.mpinLib.listUsers();
        for (var uId in idenList) {
          var uName = idenList[uId].userId;
          if (uName === this.opts.prerollid) {
            this.saveIdentity(uName);
            this.setIdentity(uName, true, function () {
              self.renderLogin.call(self);
            });
            return;
          }
        }

        this.actionSetupHome(this.opts.prerollid);
        return;
      }
    }

    var identity = this.readIdentity();

    if (identity) {
      totalAccounts = this.mpinLib.listUsers();
      totalAccounts = totalAccounts.length;
    } else {
      totalAccounts = 0;
    }

    //landingPage option
    if (this.opts.landingPage && totalAccounts === 0) {
      if (this.cfg.renderMap[this.opts.landingPage]) {
        this[this.cfg.renderMap[this.opts.landingPage]]();
        return;
      }
    }

    if (this.opts.mobileOnly) {
      this.renderMobile();
      return;
    }

    if (totalAccounts >= 1) {
      this.renderLogin();
      return;
    }


    if (this.opts.mobileSupport && this.opts.requestOTP !== "1") {
      this.renderMobile();
    } else {
      this.renderHome();
    }
  };

  mpin.prototype.renderHome = function () {
    var callbacks = {}, self = this;
    //not set landingPage
    if (this.opts.prerollid && !this.opts.landingPage) {
      var userId = self.readIdentity();
      //check if this identity is not register already !!!
      if (userId !== this.opts.prerollid) {
        this.actionSetupHome(this.opts.prerollid);
        return;
      }
    }

    if (this.opts.mobileOnly) {
      self.renderMobile.call(self);
      return;
    } else if (!this.opts.mobileSupport) {
      self.renderDesktop.call(self);
      return;
    }

    callbacks.mpin_desktop = function () {
      self.renderDesktop.call(self);
    };
    callbacks.mpin_mobile = function () {
      self.renderMobile.call(self);
    };
    callbacks.mpin_help = function () {
      self.lastView = "renderHome";
      self.toggleHelp.call(self);
      self.renderHelpTooltip.call(self, "home");
    };
    //mobile SUPPORT :::
    this.render('home', callbacks, {mobileSupport: this.opts.mobileSupport, mobileOnly: !this.opts.mobileOnly});
    this.clrInterval.call(this);
    if (this.opts.onLoaded) {
      this.opts.onLoaded();
    }
  };
  //new View redirect to
  //0 identity  - addIdentity
  //1 identity  - login
  mpin.prototype.renderDesktop = function () {
    var totalAccounts, identity;
    identity = this.readIdentity();

    if (identity) {
      totalAccounts = this.mpinLib.listUsers();
      totalAccounts = totalAccounts.length;
    } else {
      totalAccounts = 0;
    }

    if (totalAccounts === 0) {
      if (this.opts.prerollid) {
        this.actionSetupHome(this.opts.prerollid);
      } else {
        this.renderAddIdentity();
      }
    } else {
      this.renderLogin();
    }
  };


  mpin.prototype.clrInterval = function () {
    if (this.intervalID) {
      clearInterval(this.intervalID);
    }
    if (this.intervalID2) {
      clearTimeout(this.intervalID2);
    }

    this.mpinLib.cancelMobileAuth();
  };

  mpin.prototype.renderMobile = function () {
    var callbacks = {}, self = this;

    this.clrInterval();

    if (this.opts.requestOTP === "1") {
      this.renderMobileSetup();
      return;
    }

    callbacks.mpin_home = function (evt) {
      self.clrInterval.call(self);
      self.renderHome.call(self, evt);
    };

    callbacks.mpin_action_setup = function () {
      self.clrInterval.call(self);
      if (self.opts.mobileConfigURL) {
        self.renderMobileConfig.call(self);
      } else {
        self.renderMobileSetup.call(self);
      }
    };

    callbacks.mpin_desktop = function () {
      self.clrInterval.call(self);
      self.renderDesktop.call(self);
    };

    this.render("mobile-qr", callbacks, {mobileOnly: !this.opts.mobileOnly});

    setTimeout(function () {
      self.getQrParams.call(self);
    }, 0);
  };

  //	Access NUMBER
  mpin.prototype.renderMobileAN = function () {
    var callbacks = {}, self = this;

    this.clrInterval();

    if (this.opts.requestOTP === "1") {
      this.renderMobileSetup();
      return;
    }

    callbacks.mpin_home = function (evt) {
      self.clrInterval.call(self);
      self.renderHome.call(self, evt);
    };
    callbacks.mpin_action_setup = function () {
      self.clrInterval.call(self);
      if (self.opts.mobileConfigURL) {
        self.renderMobileConfig.call(self);
      } else {
        self.renderMobileSetup.call(self);
      }
    };
    callbacks.mpinLogo = function (evt) {
      self.clrInterval.call(self);
      self.renderHome.call(self, evt);
    };
    callbacks.mpin_desktop = function () {
      self.clrInterval.call(self);
      self.renderDesktop.call(self);
    };
    callbacks.mpin_access_help = function () {
      self.lastView = "renderMobile";
      self.toggleHelp.call(self);
      self.renderHelpTooltip.call(self, "landing1");
    };
    callbacks.mpin_helphub = function () {
      self.lastView = "renderMobile";
      self.toggleHelp.call(self);
      self.renderHelpTooltip.call(self, "landing2");
    };

    this.render("mobile", callbacks, {mobileOnly: !this.opts.mobileOnly});

    setTimeout(function () {
      self.getAccessNumber.call(self);
    }, 0);
  };



  mpin.prototype.renderHelp = function (tmplName, callbacks, tmplData) {
    var k, self = this;
    tmplData = tmplData || {};
    this.elHelp.innerHTML = this.readyHtml(tmplName, tmplData);
    //parse directly to element...//handlebars cannot parse html tags...
    document.getElementById("mpin_help_text").innerHTML = tmplData.helpText;
    for (k in callbacks) {
      if (document.getElementById(k)) {
        document.getElementById(k).addEventListener('click', callbacks[k], false);
      }
    }

    //close tooltip by pressing I
    document.getElementById("mpinInfoCloseCorner").onclick = function () {
      self.toggleHelp.call(self);
    };
  };
  mpin.prototype.renderHelpTooltip = function (helpLabel) {
    var callbacks = {}, self = this, helpText, secondBtn = "";
    callbacks.mpin_help_ok = function () {
      self.toggleHelp.call(self);
    };

    callbacks.mpin_help_more = function () {
      self.clrInterval.call(self);
      delete self.lastViewParams;
      self.toggleHelp.call(self);
      self.renderHelpHub.call(self);
    };

    if (helpLabel === "login" || helpLabel === "setup" || helpLabel === "loginerr") {
      secondBtn = hlp.text("help_text_" + helpLabel + "_button");
      if (helpLabel === "login" || helpLabel === "loginerr") {
        this.isLoginScreen = true;
        callbacks.mpin_help_second = function () {
          self.toggleHelp.call(self);
          self.renderLogin(true, "renderReactivatePanel");
        };
      } else if (helpLabel === "setup") {
        callbacks.mpin_help_second = function () {
          self.toggleHelp.call(self);
          self.renderHelpHubPage.call(self, 5);
        };
      }
    }

    helpText = hlp.text("help_text_" + helpLabel);
    this.renderHelp("help-tooltip-home", callbacks, {helpText: helpText, secondBtn: secondBtn});
  };
  mpin.prototype.renderHelpHub = function () {
    var callbacks = {}, self = this;
    callbacks.mpin_home = function () {
      self.renderHome.call(self);
    };
    callbacks.mpin_hub_li1 = function () {
      self.renderHelpHubPage.call(self, 1);
    };
    callbacks.mpin_hub_li2 = function () {
      self.renderHelpHubPage.call(self, 2);
    };
    callbacks.mpin_hub_li3 = function () {
      self.renderHelpHubPage.call(self, 3);
    };
    callbacks.mpin_hub_li4 = function () {
      self.renderHelpHubPage.call(self, 4);
    };
    callbacks.mpin_hub_li5 = function () {
      self.renderHelpHubPage.call(self, 5);
    };
    callbacks.mpin_hub_li6 = function () {
      self.renderHelpHubPage.call(self, 6);
    };
    callbacks.mpin_hub_li7 = function () {
      self.renderHelpHubPage.call(self, 7);
    };
    callbacks.mpin_hub_li8 = function () {
      self.renderHelpHubPage.call(self, 8);
    };
    callbacks.mpin_hub_li9 = function () {
      self.renderHelpHubPage.call(self, 9);
    };
    callbacks.mpin_hub_li10 = function () {
      self.renderHelpHubPage.call(self, 10);
    };
    callbacks.mpin_hub_li11 = function () {
      self.renderHelpHubPage.call(self, 11);
    };
    callbacks.mpin_close_hub = function () {
      self.renderLastView.call(self);
    };
    this.render("help-hub", callbacks);
  };
  mpin.prototype.renderHelpHubPage = function (helpNumber) {
    var callbacks = {}, self = this, tmplName;
    callbacks.mpin_help_hub = function () {
      self.renderHelpHub.call(self);
    };
    tmplName = "help-hub-" + helpNumber;
    this.render(tmplName, callbacks);
  };
  //
  mpin.prototype.renderLastView = function () {
    var param1, param2;
    //for render accounts
    this.lastViewParams || (this.lastViewParams = []);
    param1 = this.lastViewParams[0] || false;
    param2 = this.lastViewParams[1] || false;
    //call renderHome
    this[this.lastView](param1, param2);
  };
  mpin.prototype.renderAddIdentity = function (email) {
    var callbacks = {}, self = this, userId, deviceName = "", deviceNameHolder = "";
    //set Temporary params if enter email and then press tooltip without submit request...
    function setTemp () {
      self.tmp || (self.tmp = {});
      self.tmp.setupEmail = document.getElementById("emailInput").value.toLowerCase();
      if (self.opts.setDeviceName) {
        self.tmp.setupDeviceName = document.getElementById("deviceInput").value;
      }
    }

    callbacks.mpin_home = function () {
      delete self.tmp;
      self.renderHome.call(self);
    };
    callbacks.mpin_help = function () {
      setTemp();
      self.lastView = "renderAddIdentity";
      self.toggleHelp.call(self);
      self.renderHelpTooltip.call(self, "addidentity");
    };
    callbacks.mpin_helphub = function () {
      setTemp();
      self.lastView = "renderAddIdentity";
      self.renderHelpHub.call(self);
    };
    callbacks.mpin_setup = function () {
      delete self.tmp;
      self.actionSetupHome.call(self);
    };
    userId = (email) ? email : ((this.tmp && this.tmp.setupEmail) ? this.tmp.setupEmail : "");

    //one for
    if (this.opts.setDeviceName) {
      //get from localStorage - already set
      if (this.readIdentity("device")) {
        deviceName = (this.tmp && this.tmp.setupDeviceName) ? this.tmp.setupDeviceName : this.readIdentity("device");
        deviceNameHolder = deviceName;
      } else {
        //set only placeholder value
        deviceName = (this.tmp && this.tmp.setupDeviceName) ? this.tmp.setupDeviceName : "";
        deviceNameHolder = this.suggestDeviceName();
      }

      //devicename callback
      callbacks.mpin_help_device = function () {
        setTemp();
        self.lastView = "renderAddIdentity";
        self.toggleHelp.call(self);
        self.renderHelpTooltip.call(self, "devicename");
      };
    }

    this.render("add-identity", callbacks, {setDeviceName: this.opts.setDeviceName});
    //security Fixes
    var emailField = document.getElementById("emailInput");
    emailField.placeholder = hlp.text("setup_placeholder");
    emailField.value = userId;
    if (this.opts.setDeviceName) {
      var deviceNameField = document.getElementById("deviceInput");
      deviceNameField.placeholder = deviceNameHolder + " " + hlp.text("setup_device_default");
      deviceNameField.value = deviceName;
    }

    document.getElementById("emailInput").focus();
  };

  mpin.prototype.renderAddIdentity2 = function () {
    var renderElem, self = this, deviceName = "", deviceNameHolder = "";
    this.lastViewParams = [true, "renderAddIdentity2"];
    //set Temporary params if enter email and then press tooltip without submit request...
    function setTemp () {
      self.tmp || (self.tmp = {});
      self.tmp.setup2Email = document.getElementById("emailInput").value.toLowerCase();
      if (self.opts.setDeviceName) {
        self.tmp.setup2DeviceName = document.getElementById("deviceInput").value;
      }
    }

    renderElem = document.getElementById("mpin_identities");
    if (this.opts.setDeviceName) {
      if (this.readIdentity("device")) {
        deviceName = (this.tmp && this.tmp.setup2DeviceName) ? this.tmp.setup2DeviceName : this.readIdentity("device");
        deviceNameHolder = deviceName;
      } else {
        deviceName = (this.tmp && this.tmp.setup2DeviceName) ? this.tmp.setup2DeviceName : "";
        deviceNameHolder = this.suggestDeviceName();
      }
    }

    renderElem.innerHTML = this.readyHtml("add-identity-2", {setDeviceName: this.opts.setDeviceName});
    renderElem.style.top = "0px";
    addClass("mpinCurrentIden", "mpHide");
    //security Fixes
    var emailValue, emailField = document.getElementById("emailInput");
    emailField.placeholder = hlp.text("setup_placeholder");
    emailValue = (this.tmp && this.tmp.setup2Email) ? this.tmp.setup2Email : "";
    emailField.value = emailValue;
    if (this.opts.setDeviceName) {
      var deviceNameField = document.getElementById("deviceInput");
      deviceNameField.placeholder = deviceNameHolder + " " + hlp.text("setup_device_default");
      deviceNameField.value = deviceName;
    }

    if (document.getElementById("mpin_help")) {
      document.getElementById("mpin_help").onclick = function () {
        setTemp();
        self.lastView = "renderLogin";
        self.lastViewParams = [true, "renderAddIdentity2"];
        self.toggleHelp.call(self);
        self.renderHelpTooltip.call(self, "addidentity");
      };
    }
    if (document.getElementById("mpin_accounts_btn")) {
      document.getElementById("mpin_accounts_btn").onclick = function (evt) {
        self.renderLogin.call(self, true);
      };
    }

    document.getElementById("mpin_arrow").onclick = function (evt) {
      delete self.tmp;
      self.toggleButton();
      renderElem.style.top = "40px";
    };

    document.getElementById("mpin_setup").onclick = function () {
      delete self.tmp;
      self.actionSetupHome.call(self);
    };

    if (this.opts.setDeviceName && document.getElementById("mpin_help_device")) {
      document.getElementById("mpin_help_device").onclick = function () {
        setTemp();
        self.lastView = "renderLogin";
        self.lastViewParams = [true, "renderAddIdentity2"];
        self.toggleHelp.call(self);
        self.renderHelpTooltip.call(self, "devicename");
      };
    }
  };


  mpin.prototype.renderOtp = function (authData) {
    var callbacks = {}, self = this, leftSeconds, timerEl, timer2d, totalSec, timerExpire, drawTimer;
    //check if properties for seconds exist
    if (!authData.expireTime && !authData.nowTime) {
      self.error(4016);
      return;
    }

    //draw canvas Clock
    drawTimer = function (expireOn) {
      var start, diff;
      diff = totalSec - expireOn;
      start = -0.5 + ((diff / totalSec) * 2);
      start = Math.round(start * 100) / 100;
      timer2d.clearRect(0, 0, timerEl.width, timerEl.height);
      timer2d.beginPath();
      timer2d.strokeStyle = "#36424a";
      timer2d.arc(21, 21, 18, start * Math.PI, 1.5 * Math.PI);
      timer2d.lineWidth = 5;
      timer2d.stroke();
    };

    function expire () {
      leftSeconds = (leftSeconds) ? leftSeconds - 1 : Math.floor((timerExpire - (new Date())) / 1000);
      if (leftSeconds > 0) {
        if (document.getElementById("mpTimer")) {
          document.getElementById("mpin_seconds").innerHTML = leftSeconds;
          drawTimer(leftSeconds);
        } else {
          document.getElementById("mpin_seconds").innerHTML = leftSeconds + " " + hlp.text("mobileAuth_seconds");
        }

      } else {
        //clear Interval and go to OTP expire screen.
        clearInterval(self.intervalExpire);
        self.renderOtpExpire();
      }
    }

    callbacks.mpin_home = function () {
      clearInterval(self.intervalExpire);
      self.renderHome.call(self);
    };
    callbacks.mpin_cancel = function () {
      clearInterval(self.intervalExpire);
      self.renderLogin.call(self);
    };
    callbacks.mpin_help = function () {
      clearInterval(self.intervalExpire);
      self.lastView = "renderOtp";
      self.renderHelpHub.call(self);
    };

    this.render("otp", callbacks);

    document.getElementById("mpinOTPNumber").innerHTML = authData.otp;
    if (document.getElementById("mpTimer")) {
      timerEl = document.getElementById("mpTimer");
      timer2d = timerEl.getContext("2d");
    }
    timerExpire = new Date();
    totalSec = authData.ttlSeconds;
    timerExpire.setSeconds(timerExpire.getSeconds() + totalSec);

    expire();
    this.intervalExpire = setInterval(expire, 1000);
  };

  mpin.prototype.renderOtpExpire = function () {
    var callbacks = {}, self = this;
    callbacks.mpin_login_now = function () {
      self.renderLogin.call(self);
    };
    callbacks.mpin_help = function () {
      self.lastView = "renderOtpExpire";
      self.renderHelpHub.call(self);
    };
    this.render("otp-expire", callbacks);
  };
  mpin.prototype.suggestDeviceName = function () {
    var suggestName, platform, browser;
    platform = navigator.platform.toLowerCase();
//		browser = navigator.appCodeName;
    browser = navigator.userAgent;
    if (platform.indexOf("mac") !== -1) {
      platform = "mac";
    } else if (platform.indexOf("linux") !== -1) {
      platform = "lin";
    } else if (platform.indexOf("win") !== -1) {
      platform = "win";
    } else if (platform.indexOf("sun") !== -1) {
      platform = "sun";
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
    } else {
      browser = "_";
    }

    suggestName = platform + browser;
    return suggestName;
  };

  mpin.prototype.renderSetup = function (email) {
    var callbacks = {}, self = this;

    this.tmp || (this.tmp = {});
    this.tmp.email = (email && email != true) ? email : this.tmp.email;
    //text || circle
    this.setupInputType = "text";

    callbacks.mpin_home = function (evt) {
      self.renderHome.call(self, evt);
    };
    callbacks.mpin_clear = function () {
      self.addToPin.call(self, "clear_setup");
    };
    //fix login ...
    callbacks.mpin_login = function () {
      var digitLen = self.pinpadInput.length;
      if (digitLen === self.cfg.pinSize) {
        self.actionSetup.call(self, email);
      }
    };
    callbacks.mpin_helphub = function () {
      self.lastView = "renderSetup";
      delete self.lastViewParams;
      self.renderHelpHub.call(self);
    };
    callbacks.mpin_help_pinpad = function () {
      self.lastView = "renderSetup";
      delete self.lastViewParams;
      self.toggleHelp.call(self);
      self.renderHelpTooltip.call(self, "setup");
    };
    this.render("setup", callbacks, {email: this.tmp.email, pinSize: this.cfg.pinSize});
    this.enableNumberButtons(true);
    this.bindNumberButtons();

    this.display(hlp.text("pinpad_setup_screen_text2"), false);

    if (this.opts.prerollid) {
      var userId = this.identity;

      //check if this identity is not register already !!!
      if (userId === this.opts.prerollid) {
        document.getElementById("mpin_home").onclick = function () {
        };
      }
    }
  };

  mpin.prototype.renderLogin = function (listAccounts, subView) {
    var callbacks = {}, self = this;
    var identity = this.readIdentity();
    if (!identity) {
      this.renderAddIdentity();
      return;
    }

    callbacks.mpin_home = function (evt) {
      self.renderHome.call(self, evt);
    };
    callbacks.mpin_clear = function () {
      self.addToPin.call(self, "clear");
    };
    callbacks.mpin_arrow = function () {
      self.addToPin.call(self, "clear");
      self.toggleButton.call(self);
    };
    callbacks.mpin_login = function () {
      self.actionLogin.call(self);
    };
    callbacks.mpin_helphub = function () {
      self.lastView = "renderLogin";
      self.renderHelpHub.call(self);
    };
    callbacks.mpin_help_pinpad = function () {
      self.lastView = "renderLogin";
      self.toggleHelp.call(self);
      self.renderHelpTooltip.call(self, "login");
    };
    this.render("login", callbacks, {pinSize: this.cfg.pinSize});
    this.enableNumberButtons(true);
    this.bindNumberButtons();
    //fix - there are two more conditions ...
    if (listAccounts) {
      self.display(hlp.text("pinpad_default_message"));
      document.getElementById("mpinCurrentIden").innerHTML = identity;
      this.toggleButton();
      if (subView) {
        this[subView]();
      }
    } else {
      addClass("mpinUser", "mpinIdentityGradient");
      this.setIdentity(identity, true, function () {
        self.display(hlp.text("pinpad_default_message"));
      }, function () {
        return false;
      });
    }
  };

  mpin.prototype.renderExpire = function () {
    var callbacks = {}, self = this;

    callbacks.mpin_home = function (evt) {
      self.renderHome.call(self, evt);
    };

    callbacks.mpin_access = function (evt) {
      self.renderMobile.call(self, evt);
    };

    callbacks.mpin_action_setup = function () {
      if (self.opts.mobileConfigURL) {
        self.renderMobileConfig.call(self);
      } else {
        self.renderMobileSetup.call(self);
      }
    };

    callbacks.mpin_desktop = function () {
      self.renderDesktop.call(self);
    };

    callbacks.mpin_helphub = function () {
      self.lastView = "renderExpire";
      self.renderHelpHub.call(self);
    };

    this.render("an-expire", callbacks, {mobileOnly: !this.opts.mobileOnly});
    this.clrInterval.call(this);
  };

  mpin.prototype.getQrParams = function () {
    var self = this;
    this.mpinLib.getQrUrl("", function (err, data) {
      var qrElem, expireAfter;
      if (err) {
        self.error(4010);
      }

      var qrElem = document.getElementById("mp_qrcode");
      new QRCode(qrElem, {
        text: data.qrUrl,
        width: 158,
        height: 158
      });

      expireAfter = data.ttlSeconds;

      self._getAccess.call(self, expireAfter);
    });
  };

  mpin.prototype.getAccessNumber = function () {
    var self = this, drawTimer, timerEl, timer2d, totalSec, timerExpire, expire;

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
      timer2d.lineCap = "line";
      timer2d.strokeStyle = "#36424a";
      timer2d.arc(21, 21, 18, start * Math.PI, 1.5 * Math.PI);
      timer2d.lineWidth = 5;
      timer2d.stroke();
    };

    expire = function () {
      var expireAfter = Math.ceil((timerExpire - (new Date())) / 1000);
      if (expireAfter <= 0) {
        self.clrInterval.call(self);
        //	Set timer to 0
        if (document.getElementById("mpTimer")) {
          document.getElementById("mpin_seconds").innerHTML = 0;
          drawTimer(0);
        }
        setTimeout(function () {
          //final request
          self._getAccess.call(self, 3);
          self.renderExpire.call(self);
        }, self.cfg.lastRequestDelay);
      } else {
        //////////////////////////////////////////Clockwise
        ///// Check if Timer Element exist some template did not have timer
        if (document.getElementById("mpTimer")) {
          document.getElementById("mpin_seconds").innerHTML = expireAfter;
          drawTimer(expireAfter);
        } else {
          document.getElementById("mpin_seconds").innerHTML = expireAfter + " " + hlp.text("mobileAuth_seconds");
        }
      }
    };

    this.mpinLib.getAccessNumber(function (err, data) {
      if (err) {
        return self.error(4014);
      }

      document.getElementById("mpinAccessNumber").innerHTML = data.accessNumber;
      timerExpire = new Date();
      totalSec = data.ttlSeconds;
      timerExpire.setSeconds(timerExpire.getSeconds() + data.ttlSeconds);
      expire();
      self.intervalID = setInterval(expire, 1000);

      self._getAccess.call(self, totalSec);

    });
  };

  mpin.prototype._getAccess = function (totalSec) {
    var self = this;
    this.mpinLib.waitForMobileAuth(totalSec, 3, function (accErr, accData) {
      if (accErr) {
        return self.renderExpire.call(self);
      }

      self.successLogin.call(self, accData);
    }, function (accData) {
        return self.updateStatus(accData);;
    });
  };

  mpin.prototype.updateStatus = function (statusData) {
    var statusText;

    switch (statusData.status) {
      case "wid":
        statusText = "<span>Code scanned.<br/>Waiting for authentication...</span>";
        break;
      case "user":
        statusText = "<span>Authenticating user:<br/>" + statusData.userId + "</span>";
        break;
      case "expired":
        statusText = "<span>Authentication expired!</span>";
        break;
    }

    document.getElementById("mp_qrcode").removeAttribute('title');
    statusText && (document.getElementById("mp_qrcode").innerHTML = statusText);
  };

  mpin.prototype.renderMobileSetup = function () {
    var callbacks = {}, self = this, qrElem, mobileBtnText = "";
    callbacks.mpin_home = function () {
      self.renderHome.call(self);
    };
    callbacks.mpin_helphub = function () {
      self.lastView = "renderMobileSetup";
      self.renderHelpHub.call(self);
    };

    if (this.opts.requestOTP === "1") {
      callbacks.mpinbtn_mobile = function () {
        self.renderHome.call(self);
      };
      //
      mobileBtnText = hlp.text("mobileGet_button_back");
    } else {
      callbacks.mpinbtn_mobile = function () {
        self.renderMobile.call(self);
      };
      mobileBtnText = hlp.text("mobile_footer_btn2");
    }

    if (!this.opts.mobileAppFullURL) {
      return this.error(4006);
    }

    // mobile Html5 or Native APP
    if (this.opts.mobileNativeApp) {
      this.render("mobile-setup", callbacks, {mobileButtonText: mobileBtnText});
    } else {
      this.render("mobile-setup-html5", callbacks, {mobileAppFullURL: this.opts.mobileAppFullURL, mobileButtonText: mobileBtnText});
      qrElem = document.getElementById("mpin_qrcode");
      new QRCode(qrElem, {
        text: this.opts.mobileAppFullURL,
        width: 129,
        height: 129
      });
    }
  };

  mpin.prototype.renderMobileConfig = function () {
    var callbacks = {}, self = this;

    callbacks.mpin_home = function () {
      self.renderHome.call(self);
    };
    callbacks.mpin_helphub = function () {
      self.lastView = "renderMobileConfig";
      self.renderHelpHub.call(self);
    };
    callbacks.mpin_mobile_app = function () {
      self.renderMobileSetup();
    };
    callbacks.mpinbtn_mobile = function () {
      self.renderMobile.call(self);
    };
    callbacks.mpin_mobile_app = function () {
      self.renderMobileSetup();
    };
    callbacks.mpinbtn_mobile = function () {
      self.renderMobile.call(self);
    };

    this.render("mobile-setup-config", callbacks);
    new QRCode(document.getElementById("mpin_qrcode"), {
      text: this.opts.mobileConfigURL,
      width: 129,
      height: 129
    });
  };

  mpin.prototype.renderConfirmEmail = function () {
    var callbacks = {}, self = this, email;
    email = this.getDisplayName(this.identity);
    callbacks.mpin_home = function (evt) {
      self.renderHome.call(self, evt);
    };
    callbacks.mpin_helphub = function (evt) {
      self.lastView = "renderConfirmEmail";
      self.renderHelpHub.call(self);
    };
    callbacks.mpin_activate = function () {
      if (self.checkBtn(this))
        self.beforeRenderSetup.call(self, this);
    };
    callbacks.mpin_resend = function () {
      if (self.checkBtn(this))
        self.actionResend.call(self, this);
    };
    callbacks.mpin_accounts_btn = function () {
      self.renderLogin.call(self, true);
    };
    this.render("confirm-email", callbacks, {email: email});
  };

  mpin.prototype.mpinButton = function (btnElem, busyText) {
    var oldHtml = btnElem.innerHTML;
    addClass(btnElem, "mpinBtnBusy");
    btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(busyText) + "</span>";
    return {
      error: function (errorText) {
        removeClass(btnElem, "mpinBtnBusy");
        addClass(btnElem, "mpinBtnError");
        btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(errorText) + "</span>";
        setTimeout(function () {
          removeClass(btnElem, "mpinBtnError");
          btnElem.innerHTML = oldHtml;
        }, 1500);
      }, ok: function (okText) {
        removeClass(btnElem, "mpinBtnBusy");
        addClass(btnElem, "mpinBtnOk");
        btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(okText) + "</span>";
        setTimeout(function () {
          removeClass(btnElem, "mpinBtnOk");
          btnElem.innerHTML = oldHtml;
        }, 1500);
      }};
  };

  mpin.prototype.renderConfirmEmail = function (email) {
    var callbacks = {}, self = this;

    callbacks.mpin_home = function (evt) {
      self.renderHome.call(self, evt);
    };
    callbacks.mpin_helphub = function (evt) {
      self.lastView = "renderConfirmEmail";
      self.renderHelpHub.call(self);
    };
    callbacks.mpin_activate = function () {
      if (self.checkBtn(this))
        self.beforeRenderSetup.call(self, this, email);
    };
    callbacks.mpin_resend = function () {
      if (self.checkBtn(this))
        self.actionResend.call(self, this, email);
    };
    callbacks.mpin_accounts_btn = function () {
      self.renderLogin.call(self, true);
    };
    this.render("confirm-email", callbacks, {email: email});
  };

  mpin.prototype.mpinButton = function (btnElem, busyText) {
    var oldHtml = btnElem.innerHTML;
    addClass(btnElem, "mpinBtnBusy");
    btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(busyText) + "</span>";
    return {
      error: function (errorText) {
        removeClass(btnElem, "mpinBtnBusy");
        addClass(btnElem, "mpinBtnError");
        btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(errorText) + "</span>";
        setTimeout(function () {
          removeClass(btnElem, "mpinBtnError");
          btnElem.innerHTML = oldHtml;
        }, 1500);
      }, ok: function (okText) {
        removeClass(btnElem, "mpinBtnBusy");
        addClass(btnElem, "mpinBtnOk");
        btnElem.innerHTML = "<span class='btnLabel'>" + hlp.text(okText) + "</span>";
        setTimeout(function () {
          removeClass(btnElem, "mpinBtnOk");
          btnElem.innerHTML = oldHtml;
        }, 1500);
      }};
  };

//custom render
  mpin.prototype.renderAccountsPanel = function () {
    var self = this, renderElem, c = 0, defaultIdentity;
    if (!this.identity) {
      self.setIdentity(self.readIdentity(), false);
    }

    // Add logic to close the identity screen
    var menuBtn = document.getElementById('mpin_arrow');
    addClass(menuBtn, "mpinAUp");
    //inner ELEMENT
    renderElem = document.getElementById("mpin_identities");
    renderElem.innerHTML = this.readyHtml("accounts-panel", {mobileSupport: this.opts.mobileSupport});
    renderElem.style.display = "block";
    // button
    document.getElementById("mpin_add_identity").onclick = function () {
      if (document.getElementById("mpinCurrentIdentityTitle")) {
        addClass("mpinCurrentIdentityTitle", "mpHide");
      }
      self.renderAddIdentity2.call(self);
    };
    // button
    if (this.opts.mobileSupport) {
      document.getElementById("mpin_phone").onclick = function () {
        self.renderMobileSetup.call(self);
      };
    }

    //arrow show pinpad
    menuBtn.onclick = function () {
      document.getElementById('mpinUser').style.height = "";
      removeClass(menuBtn, 'mpinClose');
      //setIdentity if empty

      if (document.getElementById("mpinUser").innerHTML === "") {
        self.setIdentity(self.readIdentity(), true, function () {
          self.display(hlp.text("pinpad_default_message"));
        }, function () {
          return false;
        });
      }

      self.toggleButton.call(self);
    };
    //default IDENTITY
    var cnt = document.getElementById("mpin_accounts_list");
    defaultIdentity = this.readIdentity();
    if (defaultIdentity) {
      this.addUserToList(cnt, defaultIdentity, true, 0);
    }
    //bug1 default identity
    var userList = this.mpinLib.listUsers();
    for (var i in userList) {
      c += 1;
      if (userList[i].userId != defaultIdentity)
        this.addUserToList(cnt, userList[i].userId, false, c);
    }
  };
  mpin.prototype.renderUserSettingsPanel = function (iD) {
    var renderElem, self = this;
    //lastView settings
    this.lastViewParams = [true, "renderUserSettingsPanel"];
    this.isLoginScreen = false;
    renderElem = document.getElementById("mpinUser");
    renderElem.innerHTML = this.readyHtml("user-settings", {name: iD});
    this.lastView = "renderUserSettingsPanel";
    document.getElementById("mpin_deluser_btn").onclick = function (evt) {
      self.renderDeletePanel.call(self, iD);
    };
    document.getElementById("mpin_reactivate_btn").onclick = function (evt) {
      self.renderReactivatePanel.call(self, iD);
    };
    document.getElementById("mpin_cancel_btn").onclick = function (evt) {
      self.renderLogin.call(self, true);
    };
  };

  mpin.prototype.renderReactivatePanel = function (iD) {
    var renderElem, name, self = this;
    name = iD;
    this.lastViewParams = [true, "renderReactivatePanel"];
    renderElem = document.getElementById("mpinUser");
    renderElem.innerHTML = this.readyHtml("reactivate-panel", {name: name});
    document.getElementById("mpin_reactivate_btn").onclick = function () {
      self.mpinLib.deleteUser(iD);
      self.actionSetupHome.call(self, iD);
    };
    document.getElementById("mpin_cancel_btn").onclick = function () {
      self.renderLogin.call(self, !self.isLoginScreen);
    };
  };
  mpin.prototype.renderDeletePanel = function (iD) {
    var renderElem, name, self = this;
    name = iD;
    this.lastViewParams = [true, "renderDeletePanel"];
    renderElem = document.getElementById("mpinUser");
    addClass(renderElem, "mpPaddTop10");
    renderElem.innerHTML = this.readyHtml("delete-panel", {name: name});
    document.getElementById("mpin_deluser_btn").onclick = function (evt) {
      self.deleteIdentity(iD);
    };
    document.getElementById("mpin_cancel_btn").onclick = function (evt) {
      self.renderLogin.call(self, true);
    };
  };
  mpin.prototype.renderSetupDone = function () {
    var callbacks = {}, self = this, userId;
    userId = this.readIdentity() || this.identity;
    callbacks.mpin_home = function () {
      self.renderHome.call(self);
    };
    callbacks.mpin_login_now = function () {
      self.renderLogin.call(self);
    };
    callbacks.mpin_helphub = function () {
      self.lastView = "renderSetupDone";
      self.renderHelpHub.call(self);
    };
    this.render("setup-done", callbacks, {userId: userId});
  };
  //after warning
  mpin.prototype.renderRevokeIdentity = function (userId) {
    var callbacks = {}, self = this;
    callbacks.mpin_home = function () {
      self.renderHome.call(self);
    };

    callbacks.mp_action_go = function () {
      self.renderAddIdentity.call(self, userId);
    };

    callbacks.mpin_helphub = function () {
      self.lastView = "renderRevokeIdentity";
      self.lastViewParams = [userId];
      self.renderHelpHub.call(self);
    };
    callbacks.mpin_accounts_btn = function () {
      self.renderLogin.call(self, true);

    };
    this.render("revoke-identity", callbacks, {userId: userId});
  };

  mpin.prototype.addUserToList = function (cnt, uId, isDefault, iNumber) {
    var rowClass, self = this, name, userRow;
    rowClass = (isDefault) ? "mpinRow mpinRowActive" : "mpinRow";
    name = uId;
    userRow = document.createElement("li");
    userRow.setAttribute("data-identity", uId);
    userRow.className = rowClass;
    userRow.innerHTML = Handlebars.templates['user-row']({data: {name: name}});
    //security Fixes
    userRow.children[0].id = "mpin_settings_" + iNumber;
    userRow.children[1].title = name;
    userRow.children[1].setAttribute("alt", name);
    cnt.appendChild(userRow);
    document.getElementById("mpin_settings_" + iNumber).onclick = function (ev) {
      self.renderUserSettingsPanel.call(self, uId);
      ev.stopPropagation();
      return false;
    };
    userRow.onclick = function () {
      self.saveIdentity(uId);
      self.setIdentity(uId, true, function () {
        self.display(hlp.text("pinpad_default_message"));
      }, function () {
        self.error(4008);
        return false;
      });
      return false;
    };
    userRow.ondblclick = function () {
      self.toggleButton.call(self);
    };
  };
  //prevent mpin button multi clicks
  mpin.prototype.checkBtn = function (btnElem) {
    var btnClass = btnElem.className;
    return (btnClass.indexOf("mpinBtnBusy") === -1 && btnClass.indexOf("mpinBtnError") === -1 && btnClass.indexOf("mpinBtnOk") === -1);
  };
  mpin.prototype.renderIdentityNotActive = function (email) {
    var callbacks = {}, self = this;
    email = (email) ? email : this.identity;

    var usrState = this.mpinLib.getUser(email, "state");
    if (usrState === "ACTIVATED") {
      this.mpinLib.deleteUser(email);
      this.actionSetupHome(email);
      return;
    }

    callbacks.mp_action_home = function (evt) {
      self.renderHome.call(self, evt);
    };
    callbacks.mpin_activate_btn = function () {
      if (self.checkBtn(this))
        self.beforeRenderSetup.call(self, this, email);
    };
    callbacks.mpin_resend_btn = function () {
      if (self.checkBtn(this))
        self.actionResend.call(self, this, email);
    };
    callbacks.mpin_accounts_btn = function () {
      self.renderLogin.call(self, true);
    };
    callbacks.mpin_helphub = function () {
      self.lastView = "renderIdentityNotActive";
      self.lastViewParams = [email];
      self.renderHelpHub.call(self);
    };
    this.render("identity-not-active", callbacks, {email: email});
  };
  mpin.prototype.bindNumberButtons = function () {
    var self = this, btEls;
    btEls = document.getElementsByClassName("mpinPadBtn");
    for (var i = 0; i < btEls.length; i++) {
      btEls[i].onclick = function (el) {
        self.addToPin(el.target.getAttribute("data-value"));
        return false;
      };
    }

    this.bindKeys();
  };

  mpin.prototype.bindKeys = function () {
    var self = this, nool = function () {
    };
    if (this.opts.keyboardEnable && !this.keyprs && !this.keyprs2) {
      this.keyprs = true;
      document.removeEventListener("keypress", nool);
      document.removeEventListener("keydown", nool);
      document.addEventListener("keypress", function (evt) {
        var code = !evt.which ? evt.keyCode : evt.which;
        if (document.activeElement && document.activeElement.hasAttribute("type")) {
          return;
        }
        if (code >= 48 && code <= 57 && self.pinpadInput.length < self.cfg.pinSize) {
          self.addToPin(code - 48);
        } else if (code == 88 || code == 120 || code == 27) {
          self.addToPin('clear');
        } else if (code == 13 && self.pinpadInput.length === self.cfg.pinSize) {
          document.getElementById("mpin_login").onclick();
        }
      });
      //escape keydown
      document.addEventListener("keydown", function (evt) {
        self.keyprs2 = true;
        var code = !evt.which ? evt.keyCode : evt.which;
        if (document.activeElement && document.activeElement.hasAttribute("type")) {
          return;
        }
        if (code === 27) {
          self.addToPin("clear");
        } else if (code === 13 && self.pinpadInput.length === self.cfg.pinSize) {
          document.getElementById("mpin_login").onclick();
        } else if (code === 8) {
          self.removePin();
          evt.preventDefault();
        }
      });
    }
  };
  // remove last PIN
  mpin.prototype.removePin = function () {
    var _leng = this.pinpadInput.length, _lastPinElem, _clearType;
    _lastPinElem = document.getElementById("mpin_circle_" + (_leng - 1));
    if (_lastPinElem && _lastPinElem.childNodes[3]) {
      _lastPinElem.removeChild(_lastPinElem.childNodes[3]);
      this.pinpadInput = this.pinpadInput.slice(0, -1);
    }

    //new size - Disable Login || Clear
    if (this.pinpadInput.length === 0) {
      _clearType = (document.getElementById("mpin_arrow")) ? "clear" : "clear_setup";
      this.addToPin(_clearType);
    } else if (this.pinpadInput.length === (this.cfg.pinSize - 1)) {
      this.enableButton(false, "go");
      this.enableNumberButtons(true);
    }
  };

  mpin.prototype.enableNumberButtons = function (enable) {
    var els = document.getElementsByClassName("mpinPadBtn");
    for (var i = 0; i < els.length; i++) {
      var element = els[i];
      if (enable && !element.id) {
        element.className = "mpinPadBtn";
        element.disabled = false;
      } else if (!element.id) {
        element.className = "mpinPadBtn mpinBtnDisabled";
        element.disabled = true;
      }
    }
  };
  //
  mpin.prototype.addToPin = function (digit) {
    var digitLen;
    this.pinpadInput || (this.pinpadInput = "");
    this.pinpadInput += digit;
    digitLen = this.pinpadInput.length;
    if (this.setupInputType === "text") {
      addClass("mpin_input_text", "mpHide");
      removeClass("mpin_input_circle", "mpHide");
      removeClass("mpin_input_parent", "mpinInputError");
      this.setupInputType = "circle";
    }

    if (digitLen <= this.cfg.pinSize) {
      this.display();
    }

    if (digitLen === 1) {
      this.enableButton(true, "clear");
    } else if (digitLen === this.cfg.pinSize) {
      this.enableNumberButtons(false);
      this.enableButton(true, "go");
    }

    //click clear BUTTON
    if (digit === 'clear') {
      this.display(hlp.text("pinpad_default_message"));
      this.enableNumberButtons(true);
      this.enableButton(false, "go");
      this.enableButton(false, "clear");
      removeClass("mpin_inner_text", "mpinInputErrorText");
    } else if (digit === 'clear_setup') {
      this.display(hlp.text("pinpad_setup_screen_text2"), false);
      this.enableNumberButtons(true);
      this.enableButton(false, "go");
      this.enableButton(false, "clear");
    }
  };
  /**
   *	wrap all buttons function inside ...
   *
   * @param {type} enable
   * @param {type} buttonName
   * @returns {undefined}
   */
  mpin.prototype.enableButton = function (enable, buttonName) {
    var buttonValue = {}, _element;
    buttonValue.go = {id: "mpin_login", trueClass: "mpinPadBtn2", falseClass: "mpinPadBtn2 mpinBtnDisabled"};
    buttonValue.clear = {id: "mpin_clear", trueClass: "mpinPadBtn2", falseClass: "mpinPadBtn2 mpinBtnDisabled"};
    buttonValue.toggle = {id: "mp_toggleButton", trueClass: "mp_DisabledState", falseClass: ""};
    _element = document.getElementById(buttonValue[buttonName].id);
    if (!buttonValue[buttonName] || !_element) {
      return;
    }

    _element.disabled = !enable;
    _element.className = buttonValue[buttonName][enable + "Class"];
  };
  //showInPinPadDisplay
  mpin.prototype.display = function (message, isErrorFlag) {
    var removeCircles, self = this, textElem;
    removeCircles = function () {
      var pinSize = self.cfg.pinSize + 1, circles = [];
      for (var i = 1; i < pinSize; i++) {
        circles[i] = document.getElementById("mpin_circle_" + i);
        if (circles[i] && circles[i].childNodes[3]) {
          circles[i].removeChild(circles[i].childNodes[3]);
        }
      }
    };
    textElem = document.getElementById("mpin_inner_text");
    if (!message && !isErrorFlag) {

      var newCircle = document.createElement('div');
      newCircle.className = "mpinCircleIn";
      var circleID = "mpin_circle_" + (this.pinpadInput.length - 1);
      if (document.getElementById(circleID))
        document.getElementById(circleID).appendChild(newCircle);
    } else if (!isErrorFlag) {
      removeCircles();
      this.pinpadInput = "";
      removeClass("mpin_input_text", "mpHide");
      removeClass("mpin_inner_text", "mpinInputErrorText");
      addClass("mpin_input_circle", "mpHide");
      this.setupInputType = "text";
      if (textElem) {
        textElem.innerHTML = message;
      }
    } else {
      //error MESSAGE
      removeCircles();
      this.pinpadInput = "";
      removeClass("mpin_input_text", "mpHide");
      addClass("mpin_input_parent", "mpinInputError");
      addClass("mpin_inner_text", "mpinInputErrorText");
      addClass("mpin_input_circle", "mpHide");
      this.setupInputType = "text";
      if (textElem) {
        textElem.innerHTML = message;
      }
    }
  };
  mpin.prototype.getDisplayName = function (uId) {
    if (!uId)
      uId = this.identity;
    try {
      return JSON.parse(mp_fromHex(uId)).userID;
    } catch (err) {
      return uId;
    }
  };
  mpin.prototype.toggleButton = function () {
    var pinpadElem, idenElem, menuBtn, userArea, identity;
    pinpadElem = document.getElementById("mpin_pinpad");
    idenElem = document.getElementById("mpin_identities");
    menuBtn = document.getElementById("mpin_arrow");
    userArea = document.getElementById("mpinUser");
    if (!pinpadElem) {
      return;
    }

    //list identities
    if (menuBtn && !menuBtn.classList.contains("mpinAUp")) {
      this.lastViewParams = [true];
      addClass(userArea, "mpUserFat");
      addClass(menuBtn, "mpinClose");
      this.renderAccountsPanel();
      removeClass("mpinUser", "mpinIdentityGradient");
      //only for new design
      var titleElem = document.getElementById("mpinCurrentIdentityTitle");
      if (titleElem) {
        titleElem.innerHTML = hlp.text("identity_current_title");
        titleElem.style.lineHeight = "24px";
        addClass("mpinCurrentIden", "mpHide");
      }

    } else {
      //if identity not Active render ACTIVATE
      var usrState = this.mpinLib.getUser(this.identity, "state");

      //new flow v0.3
      if (usrState !== "REGISTERED") {
        this.renderIdentityNotActive(this.identity);
        return;
      }

      //clear padScreen on flip screens
      this.addToPin("clear");
      removeClass(userArea, "mpUserFat");
      addClass(userArea, "mpUserSlim");
      removeClass(menuBtn, "mpinAUp");
      //if come from add identity remove HIDDEN
      removeClass("mpinCurrentIden", "mpHide");
      addClass("mpinUser", "mpinIdentityGradient");
      this.lastViewParams = [false];
      //only for new design
      var titleElem = document.getElementById("mpinCurrentIdentityTitle");
      if (titleElem) {
        titleElem.innerHTML = hlp.text("login_current_label");
        titleElem.style.lineHeight = "12px";
        removeClass("mpinCurrentIden", "mpHide");
        removeClass(titleElem, "mpHide");
      }
    }
    return false;
  };

  //error PAGE
  mpin.prototype.renderError = function (error) {
    var callbacks = {}, errorMsg, self = this, errorCode = "";
    if (error === parseInt(error)) {
      if (!hlp.language) {
        hlp.language = this.cfg.language;
      }
      errorCode = (error === 4009) ? hlp.text("error_not_auth") : error;
      errorMsg = hlp.text("error_code_" + error);
    } else {
      errorMsg = error;
    }

    callbacks.mpin_cancel = function () {
      self.renderHome.call(self);
    };
    this.render("error", callbacks, {errorMsg: errorMsg, errorCode: errorCode});
  };

  mpin.prototype.actionSetupHome = function (uId) {
    var self = this, _email, _deviceName, _deviceNameInput, removeError, elems = [];

    _email = (uId) ? uId : document.getElementById("emailInput").value.toLowerCase();
    _deviceNameInput = (document.getElementById("deviceInput")) ? document.getElementById("deviceInput").value.trim() : "";

    if ((_email.length === 0 || !this.opts.identityCheckRegex.test(_email)) && !(this.opts.prerollid)) {
      document.getElementById("emailInput").focus();

      elems[0] = document.getElementsByClassName("mpinLabel")[0];
      elems[1] = document.getElementsByClassName("mpinInputAdd")[0];
      elems[2] = document.getElementById("emailInput");

      elems[3] = document.getElementById("emailInput");

      removeError = function () {
        elems[0].className = "mpinLabel";
        elems[1].className = "mpinInputAdd";
        elems[2].className = "mpinInput";

        elems[3].removeEventListener("keypress");
      };

      elems[0].className += " mpinLabelErr";
      elems[1].className += " mpinLeftErr";
      elems[2].className += " mpinInputErr";

      elems[3].removeEventListener("keypress", function () {
      });
      elems[3].addEventListener("keypress", removeError);

      return;
    }

    _deviceNameInput = (document.getElementById("deviceInput")) ? document.getElementById("deviceInput").value.trim() : "";
    //DEVICE NAME
    if (!this.readIdentity("device") && _deviceNameInput === "") {
      _deviceName = this.suggestDeviceName();
    } else if (!this.readIdentity("device") && _deviceNameInput !== "") {
      _deviceName = _deviceNameInput;
    } else if (_deviceNameInput !== this.readIdentity("device")) {
      _deviceName = _deviceNameInput;
    } else {
      _deviceName = false;
    }

    if (this.opts.setDeviceName) {
      _deviceNameInput = (_deviceNameInput === "") ? this.suggestDeviceName() : _deviceNameInput;
      this.mpinLib.makeNewUser(_email, _deviceNameInput);
      this.saveIdentity(_email, _deviceNameInput);
    } else {
      this.mpinLib.makeNewUser(_email);
      this.saveIdentity(_email);
    }

    this.mpinLib.startRegistration(_email, function (regErr, regData) {
      if (regErr && regErr.status === 403) {
        self.deleteIdentity(_email);
        self.error(4009);
        return;
      } else if (regErr) {
        self.error(4010);
        return;
      }

      self.mpinLib.confirmRegistration(_email, function (conErr, conData) {
        if (conErr) {
          self.renderConfirmEmail.call(self, _email);
          return;
        }

        self.renderSetup.call(self, _email);
      });
    });
  };

  mpin.prototype.beforeRenderSetup = function (btnElem, email) {
    var self = this, btn;

    btnElem && (btn = this.mpinButton(btnElem, "setupNotReady_check_info1"));
    this.mpinLib.confirmRegistration(email, function (conErr, conData) {
      if (conErr && conErr.type === "IDENTITY_NOT_VERIFIED") {
        btn && btn.error("setupNotReady_check_info2");
        return;
      } else if (conErr && conErr.type === "WRONG_FLOW") {
        btn && btn.error("setupNotReady_check_expire");
        return;
      } else if (conErr) {
        self.error(4013);
        return;
      }

      self.renderSetup.call(self, email);
    });
  };

  mpin.prototype.error = function (msg) {
    if (this.opts && this.opts.onError) {
      this.opts.onError(msg);
    } else if (msg === parseInt(msg)) {
      this.renderError(msg);
    } else {
      console.error("Error : " + msg);
    }
  };

  mpin.prototype.actionResend = function (btnElem, email) {
    var btn;

    btn = this.mpinButton(btnElem, "setupNotReady_resend_info1");

    this.mpinLib.restartRegistration(email, function (err, data) {
      if (err) {
        btn.error();
        return;
      }

      btn.ok("setupNotReady_resend_info2");
    });
  };

  mpin.prototype.actionSetup = function (email) {
    var userId, userPin, res;

    userId = email;
    userPin = this.pinpadInput;
    this.display("Verifying PIN...");
    res = this.mpinLib.finishRegistration(userId, userPin);

    if (res && res.code) {
      console.log("error");
    } else {
      this.successSetup();
    }
  };

  mpin.prototype.actionLogin = function () {
    var self = this, usrIdentity, pinValue, authFunc;
    usrIdentity = this.readIdentity();
    pinValue = this.pinpadInput;
    this.enableNumberButtons(false);
    this.enableButton(false, "go");
    this.enableButton(false, "clear");
    this.enableButton(true, "toggle");
    this.display(hlp.text("authPin_pleasewait"));

    authFunc = (self.opts.requestOTP) ? this.mpinLib.finishAuthenticationOtp : this.mpinLib.finishAuthentication;

    authFunc.call(this.mpinLib, usrIdentity, pinValue, function (authErr, authData) {
      var usrState;
      if (authErr) {
        switch (authErr.type) {
          case "WRONG_PIN":
            usrState = self.mpinLib.getUser(usrIdentity, "state");
            if (usrState === "BLOCKED") {
              var iD = self.readIdentity();
              self.deleteIdentity(iD, true);
              if (self.opts.onAccountDisabled) {
                self.opts.onAccountDisabled(iD);
              }
              return;
            } else {
              self.display(hlp.text("authPin_errorInvalidPin"), true);
            }
            break;
          case "IDENTITY_NOT_AUTHORIZED":
            self.display(hlp.text("authPin_errorNotAuthorized"), true);
            break;
          case "REQUEST_EXPIRED":
            self.display(hlp.text("authPin_errorExpired"), true);
            break;
          default:
            self.display(hlp.text("authPin_errorServer"), true);
        }

        self.enableNumberButtons(true);
        self.enableButton(false, "go");
        self.enableButton(false, "clear");
        self.enableButton(true, "toggle");
        return;
      }

      if (self.opts.requestOTP) {
        self.renderOtp(authData);
        return;
      }

      self.successLogin(authData);
    });
  };

  mpin.prototype.setIdentity = function (newIdentity, requestPermit, onSuccess, onFail) {
    var accId, self = this;

    if ((typeof (newIdentity) === "undefined") || (!newIdentity)) {
      this.identity = "";
    } else {
      this.identity = newIdentity;
    }
    accId = document.getElementById('mpinCurrentIden');
    if (accId) {
      accId.innerHTML = this.identity;
      accId.setAttribute("title", this.identity);
    }

    // no Identity go to setup HOME
    if (!this.identity) {
      this.renderAddIdentity();
      return;
    }

    if (requestPermit) {
      var usrState = this.mpinLib.getUser(newIdentity, "state");

      //new flow v0.3
      if (usrState !== "REGISTERED") {
        this.renderIdentityNotActive(newIdentity);
        return;
      }

      this.addToPin("clear");
      this.display(hlp.text("pinpad_initializing"), false);
      this.enableNumberButtons(false);
      this.enableButton(false, "go");
      this.enableButton(false, "clear");
      this.enableButton(true, "toggle");
      this.requestPermit(newIdentity, function () {
        self.enableNumberButtons(true);
        onSuccess();
      }, function (message, statusCode) {
        if (statusCode === 404) {
          self.renderIdentityNotActive(self.identity);
          onFail();
        } else {
          // Fatal server error!
          // Error getting permit 500
          self.display(hlp.text("pinpad_errorTimePermit") + " " + statusCode, true);
          self.error("Error getting the time permit.", statusCode);
          onFail();
        }
      });
    }
  };

  mpin.prototype.successSetup = function (authData) {
    var self = this;
    if (this.opts.successSetupURL) {
      window.location = this.opts.successSetupURL;
    } else if (this.opts.onSuccessSetup) {
      this.opts.onSuccessSetup(authData, function () {
        self.renderSetupDone.call(self);
      });
    } else {
      this.renderSetupDone();
    }
  };

  mpin.prototype.requestPermit = function (identity, onSuccess, onFail) {
    var usrId;

    usrId = this.readIdentity();
    this.mpinLib.startAuthentication(usrId, function (err, authData) {
      if (err) {
        return onFail(err, err.code);
      }

      onSuccess();
    });
  };

  mpin.prototype.deleteIdentity = function (iID, renderWarningFlag) {
    var delIden, usrList, newIdentity = "", self = this;
    delIden = this.mpinLib.deleteUser(iID);

    usrList = this.mpinLib.listUsers();
    for (var i in usrList) {
      newIdentity = usrList[i].userId;
      break;
    }

    if (newIdentity) {
      this.saveIdentity(newIdentity);
      this.setIdentity(newIdentity, true, function () {
        self.display(hlp.text("pinpad_default_message"));
      }, function () {
        self.error(4008);
      });

      if (!renderWarningFlag) {
        this.renderLogin(true, "renderAccountsPanel");
      }
    } else {
      this.saveIdentity("");
      this.setIdentity(newIdentity, false);
      this.identity = "";
      if (!renderWarningFlag) {
        this.renderAddIdentity();
      }
    }

    //check
    if (renderWarningFlag) {
      this.renderRevokeIdentity(iID);
    }
  };

  //recover default Identity
  // & delete it
  mpin.prototype.recover = function () {
    var oldData, defaultId, deviceId;
    oldData = JSON.parse(localStorage.getItem("mpin"));

    if (!oldData || oldData.version !== "0.3" || !oldData.defaultIdentity) {
      return;
    }

    defaultId = oldData.defaultIdentity;
    defaultId = (JSON.parse(this.mpinLib.fromHex(defaultId))).userID;

    if (oldData.deviceName) {
      deviceId = oldData.deviceName;
      this.saveIdentity(defaultId, deviceId);
    } else {
      this.saveIdentity(defaultId);
    }
    //remove from storage ...
    if (!oldData.accounts) {
      localStorage.removeItem("mpin");
    } else {
      delete oldData.defaultIdentity;
      delete oldData.version;
      oldData.deviceName && delete oldData.deviceName;
      localStorage.setItem("mpin", JSON.stringify(oldData));
    }
  };

  mpin.prototype.storeLabel = "pinpad";

  //save {identity ; device}
  mpin.prototype.saveIdentity = function (userId, deviceId) {
    var saved;

    saved = this.readIdentity("all");
    saved || (saved = {version: "4"});

    if (userId || userId === "") {
      saved.identity = userId;
    }
    if (deviceId) {
      saved.device = deviceId;
    }

    localStorage.setItem(this.storeLabel, JSON.stringify(saved));
  };

  //by default return identity
  mpin.prototype.readIdentity = function (opts) {
    var rData;
    rData = JSON.parse(localStorage.getItem(this.storeLabel));

    if (!rData) {
      return "";
    }

    if (!opts) {
      rData = rData.identity;
    } else if (opts && opts === "device") {
      rData = rData.device;
    }

    return rData;
  };

  mpin.prototype.successLogin = function (authData) {
    if (this.opts.successLoginURL) {
      window.location = this.opts.successLoginURL;
    } else if (this.opts.onSuccessLogin) {
      this.opts.onSuccessLogin(authData);
    }
  };

  function mp_fromHex (s) {
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

  //loader
  loader = function (url, type, callback) {
    if (type === "css") {
      var script = document.createElement('link');
      script.setAttribute('rel', 'stylesheet');
      script.setAttribute('type', 'text/css');
      script.setAttribute('href', url);
    } else if (type === "js") {
      var script = document.createElement('script');
      script.type = 'text/javascript';
      script.src = url;
    }
    //IE feature detect
    if (script.readyState) {
      script.onreadystatechange = callback;
    } else {
      script.onload = callback;
    }
    document.getElementsByTagName('head')[0].appendChild(script);
  };
  function addClass (elId, className) {
    var el;
    if (typeof (elId) === "string") {
      el = document.getElementById(elId);
    } else {
      el = elId;
    }

    if (el && el.className) {
      var cNames = el.className.split(/\s+/g);
      if (cNames.indexOf(className) < 0)
        el.className += " " + className;
    } else if (el)
      el.className = className;
  }
  ;
  function hasClass (elId, className) {
    var el;
    if (typeof (elId) == "string")
      el = document.getElementById(elId);
    else
      el = elId;
    var cNames = el.className.split(/\s+/g);
    return (cNames.indexOf(className) >= 0)
  }
  ;
  function removeClass (elId, className) {
    var el;
    if (typeof (elId) == "string")
      el = document.getElementById(elId);
    else
      el = elId;
    if ((el) && (el.className.indexOf(className) !== -1)) {
      var cNames = el.className.split(/\s+/g);
      cNames.splice(cNames.indexOf(className), 1);
      el.className = cNames.join(" ");
    }
  }
  ;
  //private variable
  //en
  lang.en = {};
  lang.en = {
    "pinpad_initializing": "Initializing...",
    "pinpad_errorTimePermit": "ERROR GETTING PERMIT:",
    "home_alt_mobileOptions": "Mobile Options",
    "home_button_authenticateMobile": "Authenticate <br/>with your Smartphone",
    "home_button_authenticateMobile_description": "Get your Mobile Access Number to use with your M-Pin Mobile App to securely authenticate yourself to this service.",
    "home_button_getMobile": "Get",
    "home_button_getMobile1": "M-Pin Mobile App",
    "mobile_button_setup": "Setup your phone",
    "home_button_getMobile_description": "Install the free M-Pin Mobile App on your Smartphone now!  This will enable you to securely authenticate yourself to this service.",
    "home_button_authenticateBrowser": "Authenticate <br/>with this Browser",
    "home_button_authenticateBrowser_description": "Enter your M-PIN to securely authenticate yourself to this service.",
    "home_button_setupBrowser": "Add an <br/>Identity to this Browser",
    "home_button_setupBrowser_description": "Add your Identity to this web browser to securely authenticate yourself to this service using this machine.",
    "mobileGet_header": "GET M-PIN MOBILE APP",
    "mobileGet_header2": "Get M-Pin mobile app",
    "mobileConfig_header": "Configure the M-Pin App",
    "mobileGet_text1": "Scan this QR Code or open this URL on your Smartphone:",
    "mobileGet_text2": "or open this URL on your mobile:",
    "mobileConfig_text": "Scan the QR code using the M-Pin App to configure it to use this service.",
    "mobileConfig_link": "Click here to get the M-Pin App",
    "mobileSet_app_title": "Coming soon on:",
    "mobileSet_apps": "Windows Phone",
    "mobileSet_app2": "Android",
    "mobileSet_app3": "Windows Phone",
    "mobileSet_note": "NOTE: See the following link regarding Apple's requirements for app store links:",
    "mobileGet_button_back": "Back",
    "mobileAuth_header": "AUTHENTICATE WITH YOUR M-PIN",
    "mobileAuth_seconds": "seconds",
    "mobileAuth_text1": "Your Access Number is:",
    "mobile_accessNumber_text": "Your access number is:",
    "mobileAuth_text2": "Note: Use this number in the next",
    "mobileAuth_text3": "with your M-Pin Mobile App.",
    "mobileAuth_text4": "Warning: Navigating away from this page will interrupt the authentication process and you will need to start again to authenticate successfully.",
    "otp_text1": "Your One-Time Password is:",
    "otp_signin_header": "Sign in with One-Time Password",
    "otp_text2": "Note: The password is only valid for<br/>{0} seconds before it expires.", // {0} will be replaced with the max. seconds
    "otp_seconds": "Remaining: {0} sec.", // {0} will be replaced with the remaining seconds
    "otp_expired_header": "Your One-Time Password has expired.",
    "otp_expired_button_home": "Login again to get a new OTP",
    "login_current_label": "Sign in as:",
    "setup_header": "ADD AN IDENTITY TO THIS DEVICE",
    "setup_header2": "Add an identity",
    "setup_screen_header": "Creating ",
    "setup_text1": "Enter your email address:",
    "setup_label1": "Email address:",
    "setup_label2": "Device name:",
    "setup_placeholder": "Enter your Email",
    "setup_text2": "Your email address will be used as your identity when M-Pin authenticates you to this service.",
    "setup_error_unathorized": "{0} has not been registered in the system.", // {0} will be replaced with the userID
    "setup_error_server": "Cannot process the request. Please try again later.",
    "setup_error_signupexpired": "Your signup request has been expired. Please try again.",
    "setup_button_setup": "Setup M-Pin",
    "setupPin_header": "Create your M-Pin with {0} digits", // {0} will be replaced with the pin length
    "setupPin_header2": "Setup your PIN", // {0} will be replaced with the pin length
    "setupPin_initializing": "Initializing...",
    "setupPin_pleasewait": "Please wait...",
    "setupPin_button_clear": "Clear",
    "setupPin_button_done": "Setup<br />Pin",
    "setupPin_errorSetupPin": "ERROR SETTING PIN: {0}", // {0} is the request status code
    "setupDone_header": "Congratulations!",
    "setupDone_text1": "Your M-Pin identity:",
    "setupDone_text2": "is setup, you can now sign in.",
    "setupDone_text3": "",
    "setupDone_button_go": "Sign in now with your new M-Pin!",
    "setupDone_button_go2": "Sign in now",
    "setupReady_header": "VERIFY YOUR IDENTITY",
    "setup_new_identity_title": "Setup new identity...",
    "setupReady_text1": "Your M-Pin identity",
    "setupReady_text2": "is ready to setup, now you must verify it.",
    "setupReady_text3": "We have just sent you an email, simply click the link to verify your identity.",
    "setupReady_button_go": "Verified your identity? <br/>Setup your M-Pin now",
    "setupReady_button_resend": "Not received the email? <br/>Send it again",
    "setupNotReady_header": "IDENTITY ACTIVATION REQUIRED",
    "setupNotReady_text1": "Your M-Pin identity:",
    "setupNotReady_text2": "has not been activated via the M-Pin email we sent you.",
    "setupNotReady_text3": "You need to click the link in the email we sent you, and then choose 'Setup M-Pin'.",
    "setupNotReady_check_info1": "Checking",
    "setupNotReady_check_expire": "Identity expired!",
    "setupNotReady_check_info2": "Identity not verified!",
    "setupNotReady_resend_info1": "Sending email",
    "setupNotReady_resend_info2": "Email sent!",
    "setupNotReady_resend_error": "Sending email failed!",
    "setupNotReady_button_check": "I've activated, check again",
    "setupNotReady_button_check2": "I confirmed my email",
    "setupNotReady_button_resend": "Send me the email again",
    "setupNotReady_button_resend2": "Resend confirmation email",
    "setupNotReady_button_back": "Go to the identities list",
    "authPin_header": "Enter your M-Pin",
    "authPin_button_clear": "Clear",
    "authPin_button_login": "Login",
    "authPin_pleasewait": "Authenticating...",
    "authPin_success": "Success",
    "authPin_errorInvalidPin": "Incorrect PIN!",
    "authPin_errorNotAuthorized": "You are not authorized!",
    "authPin_errorExpired": "The auth request expired!",
    "authPin_errorServer": "Server error!",
    "deactivated_header": "SECURITY ALERT",
    "deactivated_text1": "has been de-activated and your M-Pin token has been revoked.",
    "deactivated_text2": "To re-activate your identity, click on the button below to register again.",
    "revoke_text1": "You have entered your pin incorrect 3 times.",
    "revoke_text2": "Your M-Pin identity:",
    "revoke_text3": "has been revoked.",
    "deactivated_button_register": "Register again",
    "deactivated_button_register2": "Register new identity",
    "deactivated_button_back": "Back to identity list",
    "account_button_addnew": "Add a new identity to this list",
    "account_button_add": "Add new identity",
    "account_button_delete": "Remove this M-Pin Identity from this browser",
    "account_button_delete2": "Remove Identity",
    "account_button_reactivate": "Forgot my PIN. Send me a new activation email.",
    "account_button_reactivate2": "Reset PIN",
    "account_button_backToList": "Go back to identity list",
    "account_button_backToList2": "Back to identity list",
    "account_button_cancel": "Cancel and go back",
    "account_button_cancel2": "Cancel",
    "account_delete_question": "Are you sure you wish to remove this M-Pin Identity from this browser?",
    "account_delete_button": "Yes, remove this M-Pin Identity",
    "account_delete_button2": "Yes, Remove it",
    "account_reactivate_question": "Are you sure you wish to reactivate this M-Pin Identity?",
    "account_reactivate_button": "Yes, reactivate this M-Pin Identity",
    "account_reactivate_button2": "Yes, Reactivate it",
    "noaccount_header": "No identities have been added to this browser!",
    "noaccount_button_add": "Add a new identity",
    "home_intro_text": "First let's establish truth to choose the best way for you to access this service:",
    "home_intro_text2": "Choose a sign in option:",
    "signin_btn_desktop1": "Sign in with Browser",
    "signin_btn_desktop2": "(This is a PERSONAL device I DO trust)",
    "signin_btn_desktop3": "Sign in with browser",
    "signin_btn_mobile1": "Sign in with Smartphone",
    "signin_mobile_btn_text": "Sign in with your Smartphone",
    "signin_mobile_header": "Sign in with your phone",
    "scan_mobile_header": "Scan with the M-Pin app",
    "signin_mobile_btn_text2": "Sign in with phone",
    "signin_button_mobile": "Sign in with Phone",
    "signin_btn_mobile2": "(This is a PUBLIC device I DO NOT trust)",
    "home_txt_between_btns": "or",
    "home_hlp_link": "Not sure which option to choose?",
    "mobile_header_txt1": "I",
    "mobile_header_donot": "DON'T",
    "mobile_header_do": "DO",
    "mobile_header_txt3": "trust this computer",
    "mobile_header_txt4": "Sign in with Smartphone",
    "mobile_button_signin": "Sign in with this device",
    "mobile_button_signin2": "Sign in from here",
    "mobile_header_access_number": "Your Access Number is",
    "identity_current_title": "Change identity:",
    "help_ok_btn": "Ok, Got it",
    "help_more_btn": "I'm not sure, tell me more",
    "help_hub_title": "M-Pin Help Hub",
    "help_hub_li1": "What’s the difference between browser and smartphone authentication?",
    "help_hub_li2": "What should I do if I don’t have a smartphone and I don’t trust this computer?",
    "help_hub_li3": "What happens if I forget my PIN?",
    "help_hub_li4": "What happens if someone sneaks my PIN?",
    "help_hub_li5": "How should I choose my PIN number?",
    "help_hub_li6": "Can I set the same PIN on all devices every time?",
    "help_hub_li7": "How can a 4 digit PIN be more secure than a long complex password?",
    "help_hub_li8": "Should I change my PIN often?",
    "help_hub_li9": "Does the server know my PIN?",
    "help_hub_li10": "Why do I have to register from each device and browser?",
    "help_hub_li11": "Why do I have to register from each app and browser?",
    "help_hub_button": "Exit Help Hub and return to previous page",
    "help_hub_button2": "Exit Help Hub",
    "help_hub_1_p1": "The browser authentication logs you in to your account on a desktop browser using M-Pin two-factor authentication.",
    "help_hub_1_p2": "With smartphone authentication you use M-Pin Mobile app as a portable ID card to two-factor authenticate out-of-band to a desktop or browser on any external machine.",
    "help_hub_2_p1": "You can still use the browser log in, but if you are on a shared computer or feel the machine is not secure, we advise you remove the identity from the browser after you’ve completed your session.",
    "help_hub_2_p2": "",
    "help_hub_3_p1": "You will simply need to provide an",
    "help_hub_3_p11": "email address",
    "help_hub_3_p12": "in order to set up your identity. You will receive an activation email to complete the set up process.",
    "help_hub_3_p2": "You will also need to create a PIN number, this will be a secret",
    "help_hub_3_p21": "4 digit",
    "help_hub_3_p22": "code known only to you which you will use to login to the service.",
    "help_hub_4_p1": "Your PIN can only be used from a machine and browser you’ve previously registered from. If you feel your PIN could be reused from the same machine, simply follow the instructions to reset it clicking the “Forgot my PIN button”.",
    "help_hub_4_p2": "",
    "help_hub_5_p1": "You can choose any PIN number, and reuse it across different devices, without compromising the security of your credentials. With M-Pin there is no need of complex rules to choose a password, just pick an easy to remember value.",
    "help_hub_5_p2": "",
    "help_hub_6_p1": "Yes, you can use the same PIN for different accounts, different machines and different browsers, across mobile and desktop, without affecting the security of M-Pin Authentication.",
    "help_hub_6_p2": "",
    "help_hub_7_p1": "M-Pin is two-factor authentication, meaning we save “something” in your browser and mobile app to recognise you every time you access a service. The PIN number (something you know) works only with that something you have in your browser or mobile app.",
    "help_hub_7_p2": "Unlike password based authentication, M-Pin is an online authentication protocol, so it’s resistant to brute force attacks, which means you can use a low entropy PIN number and still be secure.",
    "help_hub_8_p1": "There is no need for the PIN to be changed periodically, since only you know your PIN, and it is not stored anywhere. However, changing the PIN is a simple operation. Simply click on the 'Forgot my PIN' button and follow the instructions.",
    "help_hub_8_p2": "",
    "help_hub_9_p1": "No, only you know your PIN number. It is not stored or saved anytime or anywhere during the course of operation. Your PIN is used to re-assemble a cryptographic key, and re-assembly only happens transiently, when you authenticate.",
    "help_hub_9_p2": "The cryptographic key used in authentication is not stored or saved anytime or anywhere during the course of operation.",
    "help_hub_10_p1": "To use M-Pin, you need your PIN number together with “something” saved in your browser, so you need to register from each browser to have that “something” and be able to authenticate with M-Pin.",
    "help_hub_10_p2": "",
    "help_hub_11_p1": "To use M-Pin, you need your PIN number together with 'something' saved in your browser or app, so you need to register from each browser to have that 'something', combined with your PIN number (something you know), to be able to authenticate with M-Pin.",
    "help_hub_11_p2": "",
    "help_hub_return_button": "Return to Help Hub",
    "activate_header": "ACTIVATE YOUR IDENTITY",
    "activate_header2": "Creating new identity...",
    "activate_text1": "Your M-Pin identity:",
    "activate_text2": "is ready to setup.",
    "activate_text3": "We have just send you an email, simply click the link in the email to activate your identity.",
    "activate_text_new1": "We have sent you an email to:",
    "activate_text_new2": "Click the link in the email, to confirm your identity and proceed.",
    "activate_btn1": "Activated your identity via email? Setup your M-Pin now",
    "activate_btn2": "Not received the activation email? Send it again!",
    "activate_btn_new1": "I confirmed my email",
    "activate_btn_new2": "Resend confirmation email",
    "settings_title": "IDENTITY OPTIONS",
    "settings_title2": "Edit identity",
    "landing_button_newuser": "I'm new to M-Pin, get me started",
    "revoke_header_text": "Revoking identity ...",
    "mobile_header": "GET THE M-PIN SMARTPHONE APP",
    "mobile_footer_btn": "Now, sign in with your Smartphone",
    "mobile_footer_btn2": "Sign in with Phone",
    "pinpad_setup_screen_text": "CREATE YOUR M-PIN:<br> CHOOSE 4 DIGIT",
    "pinpad_setup_screen_text2": "Setup your PIN",
    "pinpad_default_message": "Enter your PIN",
    "setup_device_label": "Choose your device name:",
    "setup_device_default": "(default name)",
    "help_text_1": "Simply choose a memorable <b>[4 digit]</b> PIN to assign to this identity by pressing the numbers in sequence followed by the 'Setup' button to setup your PIN for this identity",
    //2A
    "help_text_landing1": "This Access Number allows you to sign in with M-Pin from your smartphone.  Enter the Access Number into the M-Pin app installed on your Smartphone when prompted and follow the instructions to sign into a browser session. This number is valid for 99 seconds, once this expires a new Access number will be generated.",
    "help_text_landing2": "If you have a smartphone and are signing someone else’s device or a public computer, then please: <br>1. Download the ‘M-Pin Smartphone App’ <br> 2. Open the App and follow the steps to sign in, this will tell you when you need to enter the access code.",
    "help_text_login": "Simply enter your <span class=mpinPurple>[4 digit]</span> PIN that you assigned to this identity by pressing the numbers in sequence followed by the ‘Sign in’ button. If you have forgotten your PIN, then you can reset it by clicking the ‘Reset PIN’ button below.",
    "help_text_login_button": "Reset my PIN",
    "help_text_setup": "Simply choose a memorable <span class=mpinPurple>[4 digit]</span> PIN to assign to this identity by pressing the numbers in sequence followed by the ‘Setup’ button to setup your PIN for this identity.",
    "help_text_setup_button": "Advice on choosing PIN",
    "help_text_addidentity": "Your <span class=mpinPurple>[email address]</span> will be used as your identity when M-Pin signs you into this service.<br>You will receive an activation email to the address you provide.",
    "help_text_loginerr": "You have entered your PIN incorrectly.<br><br>You have 3 attempts to enter your PIN, after 3 incorrect attempts your identity will be removed and you will need to re-register.",
    "help_text_loginerr_button": "I've forgotton my PIN",
    "otp_header_btn_text": "Your One-time Password is:",
    "back_identity_btn": "Back to choose identity",
    "otp_under_btn_text": "Note: The password is only valid for 99 seconds before it expries.",
    "otp_remain_text": "Remaining:",
    "otp_expire_header": "Your One-Time Password has expired.",
    "otp_expire_btn": "Login again to get a new OTP.",
    "an_expire_header": "Your Access Number has expired.",
    "an_btn_generate": "Generate a new one",
    "an_btn_phone": "Setup Your Phone",
    "help_text_devicename": "This <span class=mpinPurple>[device name]</span> will be used to identify this device and the identities you create from here",
    "help_text_home": "If you are signing into your own personal device like your computer or tablet then you can ‘Sign in with Browser’, but if you are using someone else’s device or a public computer, then ‘Sign in with Smartphone’ is recommended for additional security.",
    "error_page_title": "Error page:",
    "error_page_code": "Error code:",
    "error_page_button": "Back",
    "button_back_text": "Back",
    "error_page_error": "Error:",
    "error_code_4001": "We are experiencing a technical problem. Please try again later or contact the service administrator.",
    "error_code_4002": "We are experiencing a technical problem. Please try again later or contact the service administrator.",
    "error_code_4003": "We are experiencing a technical problem. Please try again later or contact the service administrator.",
    "error_code_4004": "We detected you are using a non-compatible browser.<br> Please visit <a href='http://www.miracl.com/browser-compatibility-page'>our browser compatibility page</a> for more info.",
    "error_code_4005": "We detected you are using a non-compatible browser.<br> Please visit <a href='http://www.miracl.com/browser-compatibility-page'>our browser compatibility page</a> for more info.",
    "error_code_4006": "We are experiencing a technical problem. Please try again later or contact the service administrator.",
    "error_code_4007": "We are experiencing a technical problem. Please try again later or contact the service administrator.",
    "error_code_4008": "We are experiencing a technical problem. Please try again later or contact the service administrator.",
    "error_code_4009": "We could not complete your registration. Please contact the service administrator or try again later.", //403
    "error_code_4010": "We could not complete your registration. Please contact the service administrator or try again later.", //
    "error_code_4011": "We are experiencing a technical problem. Please try again later or contact the service administrator.", //
    "error_code_4012": "We could not complete your authentication request. Please contact the service administrator.", //
    "error_code_4013": "We could not complete your registration. Please contact the service administrator or try again later.", //
    "error_code_4014": "We are experiencing a technical problem. Please try again later or contact the service administrator.", //
    "error_code_4015": "We are experiencing a technical problem. Please try again later or contact the service administrator.", //
    "error_code_4016": "We are experiencing a technical problem. Please try again later or contact the service administrator.", //
    "error_not_auth": "You are not authorized.", //
    "pinpad_btn_login": "Login", //
    "pinpad_btn_clear": "Clear", //
    "pinpad_btn_setup": "Setup"  //
  };
  //	image should have config properties
  hlp.img = function (imgSrc) {
    return IMAGES_PATH + imgSrc;
  };
  //	translate
  hlp.text = function (langKey) {
    //hlp.language set inside render
    //customLanguageTexts - language
    return lang[hlp.language][langKey];
  };
})();

