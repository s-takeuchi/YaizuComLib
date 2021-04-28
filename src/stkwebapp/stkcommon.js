
// Server url
var svrUrl = ".";

// Acquired data
var statusCode = {};
var responseData = {};

////////////////////////////////////////
//
//  Functions for general purpose
//
////////////////////////////////////////
{
    function getArray(targetObject) {
        if (targetObject === undefined) {
            return null;
        }
        let targetArray = [];
        if (targetObject instanceof Array) {
            return targetObject;
        } else {
            targetArray.push(targetObject);
            return targetArray;
        }
    }

    function escapeString(tmpStr) {
        let csReplaced = tmpStr;
        csReplaced = csReplaced.replace(/&/g, "&amp;");
        csReplaced = csReplaced.replace(/</g, "&lt;");
        csReplaced = csReplaced.replace(/>/g, "&gt;");
        csReplaced = csReplaced.replace(/\"/g, "&quot;");
        csReplaced = csReplaced.replace(/\'/g, "&#39;");
        return csReplaced;
    }
}

////////////////////////////////////////
//
//  Functions for label and message
//
////////////////////////////////////////
{
    // Client messages
    let clientMsg = {};
    let clientLanguage = 0;

    addClientMessage('STKCOMMON_INPUT_UNAME_AND_PW', {'en':'<p>Input user name and password.</p>', 'ja':'<p>ユーザー名とパスワードを入力してください。</p>'});
    addClientMessage('STKCOMMON_USERNAME', {'en':'User name', 'ja':'ユーザー名'});
    addClientMessage('STKCOMMON_PASSWORD', {'en':'Password', 'ja':'パスワード'});
    addClientMessage('STKCOMMON_INCORRECT_UNPW', {'en':'<p>The user name or password is incorrect.</p>', 'ja':'<p>ユーザー名またはパスワードが不正です。</p>'});
    addClientMessage('STKCOMMON_CONNECTION_ERROR', {'en':'<p>A connection error occurred.</p>', 'ja':'<p>接続エラーが発生しました。</p>'});
    addClientMessage('STKCOMMON_INVALID_USERNAME', {'en':'<p>The user name is empty or contains fobidden character(s).</p>', 'ja':'<p>ユーザー名が未指定，または禁止文字が使われています。</p>'});
    addClientMessage('STKCOMMON_INVALID_PASSWORD', {'en':'<p>The passwprd is empty or contains forbidden character(s).</p>', 'ja':'<p>パスワードが未指定，または禁止文字が使われています。</p>'});
    addClientMessage('STKCOMMON_SERVER_ERROR', {'en':'An error occurred at server side.', 'ja':'サーバ側でエラーが発生しました。'});

    function addClientMessage(code, msg) {
        if (msg.en !== undefined && msg.ja !== undefined) {
            clientMsg[code] = msg;
        }
    }

    function deleteClientMessage(code) {
        delete clientMsg[code];
    }

    function getClientMessageEn(code) {
        return clientMsg[code].en;
    }

    function getClientMessageJa(code) {
        return clientMsg[code].ja;
    }

    function setClientLanguage(lang) {
        clientLanguage = lang;
    }

    function getClientLanguage() {
        return clientLanguage;
    }

    function getClientMessage(code) {
        if (clientLanguage == 0) {
            return clientMsg[code].en;
        }
        if (clientLanguage == 1) {
            return clientMsg[code].ja;
        }
    }

    function getSvrMsg(respDat) {
        if (getClientLanguage() == 1) {
            return getClientMessage('STKCOMMON_SERVER_ERROR') + '<br/>' + respDat.MsgJpn;
        } else {
            return getClientMessage('STKCOMMON_SERVER_ERROR') + '<br/>' + respDat.MsgEng;
        }
    }
}

////////////////////////////////////////
//
//  Functions for login and logout
//
////////////////////////////////////////
{
    // For login info
    let initLoginModalFlag = false;

    // Login ID and password
    let loginId = "";
    let loginPw = "";

    let tryLogin = function(func) {
        loginId = $("#loginId").val();
        loginPw = $("#loginPw").val();
        if (!loginId.match(/^([a-zA-Z0-9\._\-/@])+$/)) {
            $('#login_Modal_Body').empty();
            $('#login_Modal_Body').append(getClientMessage('STKCOMMON_INVALID_USERNAME'));
            return;
        }
        if (!loginPw.match(/^([a-zA-Z0-9!\?\.\+\-\$%#&\*/=@])+$/)) {
            $('#login_Modal_Body').empty();
            $('#login_Modal_Body').append(getClientMessage('STKCOMMON_INVALID_PASSWORD'));
            return;
        }
        func(loginId, loginPw);
    };

    let initLoginModal = function(func) {
        let loginModal = $('<div id="login_Modal" class="modal fade" tabindex="-1" role="dialog" data-keyboard="false" data-backdrop="static" style="z-index: 2005;">');
        let modalDialog = $('<div class="modal-dialog modal-xl">');
        let modalContent = $('<div class="modal-content">');
        let modalHeader = $('<h3 class="modal-header">Login</h3>');
        let modalBody = $('<div class="modal-body"><div"><div class="form-group"><input type="text" maxlength="31" class="form-control" id="loginId" placeholder="' + getClientMessage('STKCOMMON_USERNAME') + '" value="" /></div><div class="form-group"><input type="password" maxlength="31" class="form-control" id="loginPw" placeholder="' + getClientMessage('STKCOMMON_PASSWORD') + '" value="" /></div><div id="login_Modal_Body"></div><button id="loginButton" type="button" class="btn btn-dark">Login</button></div></div>');

        modalContent.append(modalHeader);
        modalContent.append(modalBody);
        modalDialog.append(modalContent);
        loginModal.append(modalDialog);
        $('body').append(loginModal);
        $('#login_Modal_Body').append(getClientMessage('STKCOMMON_INPUT_UNAME_AND_PW'));
        $('#loginButton').on('click', function() { tryLogin(func); });
    };

    function getUserName() {
        return loginId;
    }

    function changeLoginPassword(password) {
        loginPw = password;
        document.cookie = "loginId=" + encodeURIComponent(window.btoa(loginId)) + ";max-age=86400;samesite=strict;secure";
        document.cookie = "loginPw=" + encodeURIComponent(window.btoa(loginPw)) + ";max-age=86400;samesite=strict;secure";
    }

    function tryLogout() {
        loginId = "";
        loginPw = "";
        document.cookie = "loginId=" + loginId;
        document.cookie = "loginPw=" + loginPw;
        location.reload();
    }

    function showLoginModal(func) {
        if (initLoginModalFlag == false) {
            initLoginModalFlag = true;
            initLoginModal(func);
        }
        let myCookies = document.cookie.split(';');
        myCookies.forEach(function(value) {
            let content = value.split('=');
            if (content.length == 2 && content[0].trim() === 'loginId') {
                loginId = window.atob(decodeURIComponent(content[1]));
            }
            if (content.length == 2 && content[0].trim() === 'loginPw') {
                loginPw = window.atob(decodeURIComponent(content[1]));
            }
        })
        if (loginId !== "" && loginPw !== "") {
            func(loginId, loginPw);
            return;
        }
        $('#login_Modal').modal('show');
    }

    function setLoginResult(result) {
        if (result == 0 && $('#login_Modal').is(':hidden')) {
            return;
        }
        if (result == 0 && $('#login_Modal').is(':visible')) {
            $('#login_Modal').modal('hide');
            document.cookie = "loginId=" + encodeURIComponent(window.btoa(loginId)) + ";max-age=86400;samesite=strict;secure";
            document.cookie = "loginPw=" + encodeURIComponent(window.btoa(loginPw)) + ";max-age=86400;samesite=strict;secure";
        }
        if (result != 0 && $('#login_Modal').is(':hidden')) {
            $('#login_Modal').modal('show');
        }
        if (result != 0 && $('#login_Modal').is(':visible')) {
            $('#login_Modal_Body').empty();
            if (result == 1) {
                $('#login_Modal_Body').append(getClientMessage('STKCOMMON_INCORRECT_UNPW'));
            } else if (result == 2) {
                $('#login_Modal_Body').append(getClientMessage('STKCOMMON_CONNECTION_ERROR'));
            }
        }
    }

    function isPasswordCorrect(password) {
        if (loginPw === password) {
            return true;
        } else {
            return false;
        }
    }
}

////////////////////////////////////////
//
//  Functions for input modal
//
////////////////////////////////////////
{
    // Init flag of input modal
    let initInputModalFlag = false;

    let initInputModal = function() {
        let inputModal = $('<div id="inputDlgModal" class="modal fade" tabindex="-1" role="dialog" data-keyboard="false" data-backdrop="static" style="z-index: 2005;">');
        let modalDialog = $('<div class="modal-dialog modal-xl">');
        let modalContent = $('<div class="modal-content">');
        let modalHeader = $('<h5 class="modal-header" id="inputDlgModalTitle"/>');
        let modalBody = $('<div class="modal-body" id="inputDlgModalBody"/>');

        modalContent.append(modalHeader);
        modalContent.append(modalBody);
        modalDialog.append(modalContent);
        inputModal.append(modalDialog);
        $('body').append(inputModal);
    }

    function showInputModal(title, contents) {
        if (initInputModalFlag == false) {
            initInputModalFlag = true;
            initInputModal();
        }
        $('#inputDlgModalTitle').empty();
        $('#inputDlgModalBody').empty();
        $('#inputDlgModalTitle').append('<div style="float:left;">' + title + '</div><div class="plane-link" style="float:right;"><a href="#" onclick="closeInputModal()"><span class="icon icon-cross" style="font-size:22px;"></span></div>');
        $('#inputDlgModalBody').append(contents);
        $('#inputDlgModal').modal('show');
    }

    function closeInputModal() {
        $('#inputDlgModal').modal('hide');
    }
}

////////////////////////////////////////
//
//  Functions for main console
//
////////////////////////////////////////
{
    function initMainPage(title, iconname, contents) {
        let navBarHeader = $('<div class="d-none d-sm-block rscommand"><a class="navbar-brand" href=""><img src="' + iconname + '" width=22 height=22/><small>' + title + '</small></a></div> <div id="rsCommand"/> <div class="d-none d-lg-block" style="width:40px;"/><div class="d-none d-xl-block" style="width:40px;"/> <button type="button" class="navbar-toggler" data-toggle="collapse" data-toggle="collapse" data-target="#top-nav"><span class="navbar-toggler-icon" style="font-size:15px;"></span></button>');
        let navBarNav = $('<ul class="navbar-nav  mr-auto">');
        if (contents instanceof Array) {
            for (let key in contents) {
                if (contents[key].actApiName != null && contents[key].title != null) {
                    navBarNav.append($('<li class="nav-item"><a class="nav-link" onclick="' + contents[key].actApiName + '"> ' + contents[key].title + '</a></li>'));
                }
            }
        }
        let navBarNavRight = $('<ul class="navbar-nav"><li class="nav-item dropdown"><a href="" class="nav-link dropdown-toggle" data-toggle="dropdown">' + getUserName() + '</a><div id="rsUserMenu" class="dropdown-menu dropdown-menu-right"></div></li></u>')
        let navBarCollapse = $('<div class="collapse navbar-collapse justify-content-start" id="top-nav">');
        navBarCollapse.append(navBarNav);
        navBarCollapse.append(navBarNavRight);
        let navBarDefault = $('<nav class="navbar navbar-expand-md navbar-dark bg-dark fixed-top">');
        navBarDefault.append(navBarHeader);
        navBarDefault.append(navBarCollapse);
        let containerFluidWorkSpace = $('<div class="container">');
        $('body').append(navBarDefault);
        $('body').append(containerFluidWorkSpace);
        clearRsUserMenu();
    }

    function drowContainer(contents) {
        $('.container').remove();
        $('.container-fluid').remove();
        $('body').append('<div class="container">');
        $('.container').append(contents);
    }

    function drowContainerFluid(contents) {
        $('.container').remove();
        $('.container-fluid').remove();
        $('body').append('<div class="container-fluid" style="margin:0px;padding:0px">');
        $('.container-fluid').append(contents);
    }

    function addRsUserMenu(contents) {
        if (contents instanceof Array) {
            for (let key in contents) {
                if (contents[key].actApiName != null && contents[key].title != null) {
                    $('#rsUserMenu').prepend($('<a class="dropdown-item" onclick="' + contents[key].actApiName + '">' + contents[key].title + '</a>'));
                }
            }
        }
    }

    function clearRsUserMenu() {
        $('#rsUserMenu').empty();
        $('#rsUserMenu').append('<a class="dropdown-item" onclick="tryLogout()">Logout</a>');
    }

    function addRsCommand(func, icon, enable) {
        if (enable == true) {
            $('#rsCommand').append('<div class="rscommand"><a href="#" onclick="' + func + '">&nbsp;<span class="icon ' + icon + '" style="font-size:30px;"></span>&nbsp;</a></div>');
        } else {
            $('#rsCommand').append('<div class="rscommand">&nbsp;<span class="icon ' + icon + '" style="font-size:30px;"></span>&nbsp;</div>');
        }
    }

    function clearRsCommand() {
        $('#rsCommand').empty();
    }

    function addDropDown(dropdownTitle) {
        $('#rsCommand').append('<div class="nav-item dropdown rscommand"><a href="#" class="nav-link dropdown-toggle" id="navbarDropdown" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false"  style="padding-left:0px;padding-right:0px;margin-left:-10px">' + dropdownTitle + '</a><div id="rsNavDropDown" class="dropdown-menu" aria-labelledby="navbarDropdown"/></div>');
    }

    function changeDropDownTitle(dropdownTitle) {
        $('#navbarDropdown').text(dropdownTitle);
    }

    function getDropDownMenu(index) {
        return $('#dropDownMenu_' + index).text();
    }

    function addDropDownMenu(index, menuTitle, func) {
        $('#rsNavDropDown').append('<a id="dropDownMenu_' + index + '" class="dropdown-item" href="#" onclick="' + func + '">' + menuTitle + '</a>');
    }
}

////////////////////////////////////////
//
//  Functions for notification
//
////////////////////////////////////////
{
    function displayAlertSuccess(parent, msg) {
        $(parent + ' .alert').remove();
        $(parent).append('<div class="alert alert-success" role="alert">' + msg + '</div>');
    }

    function displayAlertDanger(parent, msg) {
        $(parent + ' .alert').remove();
        $(parent).append('<div class="alert alert-danger" role="alert">' + msg + '</div>');
    }

    function displayAlertWarning(parent, msg) {
        $(parent + ' .alert').remove();
        $(parent).append('<div class="alert alert-warning" role="alert">' + msg + '</div>');
    }

    function displayAlertInfo(parent, msg) {
        $(parent + ' .alert').remove();
        $(parent).append('<div class="alert alert-info" role="alert">' + msg + '</div>');
    }
}

////////////////////////////////////////
//
//  Functions for communication
//
////////////////////////////////////////
{
    // underComm = 0 : No communication state, >= 1 : Communication state
    let underComm = 0;

    // Init flag of loading modal
    let initLoadingModalFlag = false;

    // Timeout in milliseconds for API call
    let timeout = 99000;

    // Authentication token
    let authToken = "";

    let initLoadingModal = function() {
        let loadingModal = $('<div id="loading_Modal" class="modal fade" tabindex="-1" role="dialog" data-keyboard="false" data-backdrop="static" style="z-index: 2010;">');
        let modalDialog = $('<div class="modal-dialog modal-xl">');
        let modalContent = $('<div class="modal-content">');
        let modalHeader = $('<h5 class="modal-header">Now loading ...</h5>');
        let modalBody = $('<div class="modal-body"><div style="height:20px"><div class="progress"><div class="progress-bar bg-primary progress-bar-striped active" role="progressbar" aria-valuenow="40" aria-valuemin="0" aria-valuemax="100" style="width:100%"> </div></div></div></div>');

        modalContent.append(modalHeader);
        modalContent.append(modalBody);
        modalDialog.append(modalContent);
        loadingModal.append(modalDialog);
        $('body').append(loadingModal);
    }

    let waitForResponse = function(count, targetFunc) {
        if (initLoadingModalFlag == false) {
            initLoadingModalFlag = true;
            initLoadingModal();
        }
        if (targetFunc == null) {
            return;
        }
        if (count < 10) {
            if (underComm == 0) {
                targetFunc();
            } else {
                setTimeout(function() {waitForResponse(count + 1, targetFunc);}, 50);
            }
            return;
        }
        if (underComm == 0) {
            $('#loading_Modal').modal('hide');
            targetFunc();
            return;
        }
        $('#loading_Modal').modal('show');
        setTimeout(function() {waitForResponse(count + 1, targetFunc);}, 1000);
    }

    let waitForResponseWithoutDialog = function(count, targetFunc) {
        if (targetFunc == null) {
            return;
        }
        if (count < 10) {
            if (underComm == 0) {
                targetFunc();
            } else {
                setTimeout(function() {waitForResponseWithoutDialog(count + 1, targetFunc);}, 50);
            }
            return;
        }
        if (underComm == 0) {
            targetFunc();
            return;
        }
        setTimeout(function() {waitForResponseWithoutDialog(count + 1, targetFunc);}, 500);
    }

    let sendRequestRecvResponse = function(method, url, request, keystring, asyncFlag) {
        underComm++;
        if (method === 'GET' || method === 'DELETE') {
            $.ajax({
                type: method,
                dataType: 'json',
                url: svrUrl + url,
                data: request,
                async: asyncFlag,
                timeout: timeout,
                crossDomain: true,
                beforeSend: function( xhr, settings ) {
                    xhr.setRequestHeader('Authorization', 'Bearer ' + authToken);
                },
                success: function(msg, textStatus, xhr) {
                    statusCode[keystring] = xhr.status;
                    responseData[keystring] = {};
                    $.extend(responseData[keystring], msg);
                    underComm--;
                },
                error: function(xhr, textStatus, errorThrown) {
                    if (textStatus === 'timeout') {
                        statusCode[keystring] = -1;
                        responseData[keystring] = {};
                    } else {
                        statusCode[keystring] = xhr.status;
                        try {
                            responseData[keystring] = {};
                            $.extend(responseData[keystring], JSON.parse(xhr.responseText));
                        } catch (e) {
                            responseData[keystring] = {};
                        }
                    }
                    underComm--;
                }
            });
        } else {
            $.ajax({
                type: method,
                dataType: 'json',
                url: svrUrl + url,
                data: JSON.stringify(request),
                contentType: 'application/json',
                async: asyncFlag,
                timeout: timeout,
                crossDomain: true,
                beforeSend: function( xhr, settings ) {
                    xhr.setRequestHeader('Authorization', 'Bearer ' + authToken);
                },
                success: function(msg, textStatus, xhr) {
                    statusCode[keystring] = xhr.status;
                    responseData[keystring] = {};
                    $.extend(responseData[keystring], msg);
                    underComm--;
                },
                error: function(xhr, textStatus, errorThrown) {
                    if (textStatus === 'timeout') {
                        statusCode[keystring] = -1;
                        responseData[keystring] = {};
                    } else {
                        statusCode[keystring] = xhr.status;
                        try {
                            responseData[keystring] = {};
                            $.extend(responseData[keystring], JSON.parse(xhr.responseText));
                        } catch (e) {
                            responseData[keystring] = {};
                        }
                    }
                    underComm--;
                }
            });
        }
        return;
    }

    function setAuthenticationToken(tmpToken) {
        authToken = tmpToken;
    }

    function apiCall(method, url, request, keystring, targetFunc) {
        if (method != null && url != null && keystring !== '') {
            sendRequestRecvResponse(method, url, request, keystring, true);
        }
        if (targetFunc != null) {
            setTimeout(function() {waitForResponse(0, targetFunc);}, 1);
        }
    }

    function MultiApiCall(contents, targetFunc) {
        for (let key in contents) {
            if (contents[key].method != null && contents[key].url != null && contents[key].keystring !== '') {
                sendRequestRecvResponse(contents[key].method, contents[key].url, contents[key].request, contents[key].keystring, true);
            }
        }
        if (targetFunc != null) {
            setTimeout(function() {waitForResponse(0, targetFunc);}, 1);
        }
    }

    {
        let sContents = [];
        let sTargetFunc = null;

        function sequentialApiCall(contents, targetFunc) {
            $('#loading_Modal').modal('show');
            sequentialApiCallImpl(contents, targetFunc);
        }

        function sequentialApiCallImpl(contents, targetFunc) {
            sContents = contents;
            sTargetFunc = targetFunc;
            if (sContents instanceof Array) {
                if (sContents.length == 0) {
                    return;
                }
            } else {
                return;
            }
            let firstElem = sContents.shift();
            if (firstElem.method != null && firstElem.url != null && firstElem.keystring !== '') {
                sendRequestRecvResponse(firstElem.method, firstElem.url, firstElem.request, firstElem.keystring, true);
            }
            if (sTargetFunc != null && sContents.length == 0) {
                setTimeout(function() {waitForResponseWithoutDialog(0, sTargetFunc); $('#loading_Modal').modal('hide');}, 1000);
            } else {
                setTimeout(function() {waitForResponseWithoutDialog(0, nextSequentialApiCall);}, 1);
            }
        }

        function nextSequentialApiCall() {
            sequentialApiCallImpl(sContents, sTargetFunc);
        }
    }
}
