
// Server url
var svrUrl = ".";

// Timeout in milliseconds for API call
var timeout = 10000;

// underComm = 0 : No communication state, >= 1 : Communication state
var underComm = 0;

// [0] : for API-1, [1] : for API-2, [2] : for API-3, ...
var statusCode = {};
var responseData = {};

// For login info
var initLoginModalFlag = false;
var loginId = "";
var loginPw = "";

// Init flag of loading modal
var initLoadingModalFlag = false;

// Client messages
var clientMsg = {};
var clientLanguage = 0;

function initLoginModal(func) {
    var loginModal = $('<div id="login_Modal" class="modal fade" tabindex="-1" role="dialog" data-keyboard="false" data-backdrop="static">');
    var modalDialog = $('<div class="modal-dialog">');
    var modalContent = $('<div class="modal-content">');
    var modalHeader = $('<h3 class="modal-header">Login</h3>');
    var modalBody = $('<div class="modal-body"><div style="height:190px"><div class="form-group"><input type="email" maxlength="254" class="form-control" id="loginId" placeholder="Your Email" value="" /></div><div class="form-group"><input type="password" maxlength="31" class="form-control" id="loginPw" placeholder="Your Password" value="" /></div><div id="login_Modal_Body"></div><button type="button" class="btn btn-dark" onclick="tryLogin(' + func + ')">Login</button></div></div>');

    modalContent.append(modalHeader);
    modalContent.append(modalBody);
    modalDialog.append(modalContent);
    loginModal.append(modalDialog);
    $('body').append(loginModal);
    $('#login_Modal_Body').append('<p>Input email address and password.</p>');
}

function tryLogin(func) {
    loginId = $("#loginId").val();
    loginPw = $("#loginPw").val();
    if (!loginId.match(/^([a-zA-Z0-9])+([a-zA-Z0-9\._-])*@([a-zA-Z0-9_-])+([a-zA-Z0-9\._-]+)+$/)) {
        $('#login_Modal_Body').empty();
        $('#login_Modal_Body').append('<p>The format of the email address is incorrect.</p>');
        return;
    }
    if (!loginPw.match(/^([a-zA-Z0-9!\?\.\+\$%#&\*=@])+$/)) {
        $('#login_Modal_Body').empty();
        $('#login_Modal_Body').append('<p>The passwprd is empty or contains abnormal character(s).</p>');
        return;
    }
    if (func() == true) {
        $('#login_Modal').modal('hide');
        document.cookie = "loginId=" + encodeURIComponent(window.btoa(loginId)) + ";max-age=86400;samesite=strict;secure";
        document.cookie = "loginPw=" + encodeURIComponent(window.btoa(loginPw)) + ";max-age=86400;samesite=strict;secure";
    } else {
        $('#login_Modal_Body').empty();
        $('#login_Modal_Body').append('<p>The email address or password is incorrect.</p>');
    }
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
    var myCookies = document.cookie.split(';');
    myCookies.forEach(function(value) {
        var content = value.split('=');
        if (content.length == 2 && content[0].trim() === 'loginId') {
            loginId = window.atob(decodeURIComponent(content[1]));
        }
        if (content.length == 2 && content[0].trim() === 'loginPw') {
            loginPw = window.atob(decodeURIComponent(content[1]));
        }
    })
    if (loginId !== "" && loginPw !== "") {
        if (func() == true) {
            return;
        } else {
            $('#login_Modal').modal('show');
        }
    }
    $('#login_Modal').modal('show');
}

function initLoadingModal() {
    var loadingModal = $('<div id="loading_Modal" class="modal fade" tabindex="-1" role="dialog" data-keyboard="false" data-backdrop="static">');
    var modalDialog = $('<div class="modal-dialog">');
    var modalContent = $('<div class="modal-content">');
    var modalHeader = $('<h5 class="modal-header">Now loading ...</h5>');
    var modalBody = $('<div class="modal-body"><div style="height:20px"><div class="progress"><div class="progress-bar bg-primary progress-bar-striped active" role="progressbar" aria-valuenow="40" aria-valuemin="0" aria-valuemax="100" style="width:100%"> </div></div></div></div>');

    modalContent.append(modalHeader);
    modalContent.append(modalBody);
    modalDialog.append(modalContent);
    loadingModal.append(modalDialog);
    $('body').append(loadingModal);
}

function initMainPage(title, contents) {
    var navBarHeader = $('<a class="navbar-brand" href=""><img src="squirrel.svg" width=22 height=22><strong>' + title + '</strong></a> <div id="rsCommand"/> <button type="button" class="navbar-toggler" data-toggle="collapse" data-toggle="collapse" data-target="#top-nav"><span class="navbar-toggler-icon" style="font-size:15px;"></span></button>');
    var navBarNav = $('<ul class="navbar-nav  mr-auto">');
    if (contents instanceof Array) {
        for (var key in contents) {
            navBarNav.append($('<li class="nav-item" id="menu-' + contents[key].id + '" style="display:none"><a class="nav-link" onclick="' + contents[key].actApiName + '(\'' + contents[key].id + '\')"> ' + contents[key].title + '</a></li>'));
        }
    }
    var navBarNavRight = $('<ul class="navbar-nav"><li class="nav-item"><a class="nav-link" onclick="tryLogout()"> Logout</a></li></u>')
    var navBarCollapse = $('<div class="collapse navbar-collapse justify-content-start" id="top-nav">');
    navBarCollapse.append(navBarNav);
    navBarCollapse.append(navBarNavRight);
    var navBarDefault = $('<nav class="navbar navbar-expand-md navbar-dark bg-dark fixed-top">');
    navBarDefault.append(navBarHeader);
    navBarDefault.append(navBarCollapse);
    var containerFluidWorkSpace = $('<div class="container">');
    if (contents instanceof Array) {
        for (var key in contents) {
            containerFluidWorkSpace.append('<div><div id="' + contents[key].id + '" class="row col-xs-12" style="display:none"></div></div>');
        }
    }
    $('body').append(navBarDefault);
    $('body').append(containerFluidWorkSpace);
    $('body').css('padding-top', '70px');
}

function addRsCommand(func, icon, enable)
{
    if (enable == true) {
        $('#rsCommand').append('<a href="#" onclick="' + func + '"><span class="icon ' + icon + '" style="font-size:30px;"></span> </a>');
    } else {
        $('#rsCommand').append('<span class="icon ' + icon + '" style="font-size:30px;"></span> ');
    }
}

function clearRsCommand()
{
    $('#rsCommand').empty();
}

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

function apiCall(method, url, request, keystring, targetFunc) {
    if (method != null && url != null && keystring !== '') {
        sendRequestRecvResponse(method, url, request, keystring, true);
    }
    if (targetFunc != null) {
        setTimeout(function() {waitForResponse(0, targetFunc);}, 1);
    }
}

function MultiApiCall(contents, targetFunc) {
    for (var key in contents) {
        if (contents[key].method != null && contents[key].url != null && contents[key].keystring !== '') {
            sendRequestRecvResponse(contents[key].method, contents[key].url, contents[key].request, contents[key].keystring, true);
        }
    }
    if (targetFunc != null) {
        setTimeout(function() {waitForResponse(0, targetFunc);}, 1);
    }
}

function waitForResponse(count, targetFunc) {
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
    setTimeout(function() {waitForResponse(count + 1, targetFunc);}, 500);
}

function sendRequestRecvResponse(method, url, request, keystring, asyncFlag) {
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
                xhr.setRequestHeader('Authorization', 'Bearer ' + loginId + ' ' + loginPw);
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
                xhr.setRequestHeader('Authorization', 'Bearer ' + loginId + ' ' + loginPw);
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
