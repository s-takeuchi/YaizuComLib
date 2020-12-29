// Selected user information on user management page
var selectedUserId = -1;

// User operation result
var userOpeStatus = 0;

function initClientMessage4Um() {
    //
    // User information
    //
    addClientMessage('USERMGMT', {'en':'User Management', 'ja':'ユーザー管理'});
    addClientMessage('USERMGMTS', {'en':'User Mgmt', 'ja':'ユーザー管理'});
    addClientMessage('NOUSEREXIST', {'en':'<p>No user exists</p>', 'ja':'<p>ユーザーは存在しません</p>'});
    addClientMessage('USERNAME', {'en':'User Name', 'ja':'ユーザー名'});
    addClientMessage('USERROLE', {'en':'User Role', 'ja':'ユーザーロール'});
    addClientMessage('USERROLEADMIN', {'en':'Administrator', 'ja':'管理者'});
    addClientMessage('USERROLEUSER', {'en':'General User', 'ja':'一般ユーザー'});
    addClientMessage('USEROPECOMPLETED', {'en':'The operation has completed.', 'ja':'操作が完了しました。'});
    addClientMessage('USER_PASSWORD_ON', {'en':'Configure password', 'ja':'パスワードを設定する'});
    addClientMessage('USER_PASSWORD', {'en':'Password', 'ja':'パスワード'});
    addClientMessage('USER_CHG_PW', {'en':'Change Password', 'ja':'パスワードの変更'});
    addClientMessage('USER_CHG_PW_CURRENT', {'en':'Current Password', 'ja':'現在のパスワード'});
    addClientMessage('USER_CHG_PW_NEW', {'en':'New Password', 'ja':'新しいパスワード'});
    addClientMessage('USER_CHG_PW_CONFIRM', {'en':'Confirm New Password', 'ja':'新しいパスワード(確認用)'});
    addClientMessage('USER_PW_WRONG', {'en':'The specified password is not correct.', 'ja':'指定されたパスワードが不正です。'});
    addClientMessage('USER_NEWPW_WRONG', {'en':'The specified "New Password" and "Confirm New Password" are not matched.', 'ja':'"新しいパスワード"と"新しいパスワード(確認用)"が不一致です。'});
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

function transDisplayUser() {
    userOpeStatus = 0;
    var contents = [{ method: 'GET', url: '/api/user/?target=all', request: null, keystring: 'API_GET_USERS' }];
    MultiApiCall(contents, displayUser);
}

function displayUser() {
    var userMgmt = $('<div id="usermgmt">');
    showInputModal(getClientMessage('USERMGMT'), userMgmt);

    if (statusCode['API_GET_USERS'] == -1 || statusCode['API_GET_USERS'] == 0) {
        displayAlertDanger('#usermgmt', getClientMessage('CONNERR'));
        $('#usermgmt').append('<p><button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('DLG_CLOSE') + '</button></p>');
        return;
    } else if (statusCode['API_GET_USERS'] != 200) {
        displayAlertDanger('#usermgmt', getSvrMsg(responseData['API_GET_USERS']));
        $('#usermgmt').append('<p><button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('DLG_CLOSE') + '</button></p>');
        return;
    }
    var userList = getArray(responseData['API_GET_USERS'].Data.User);
    if (userList == null) {
        $('#usermgmt').append(getClientMessage('NOUSEREXIST'));
    }

    selectedUserId = -1;

    if (responseData['API_GET_USERS'].Data.User !== undefined) {
        var userListTable = $('<table>');
        userListTable.addClass('table table-striped');

        var tHead = $('<thead class="thead-dark">');
        tHead.append('<tr><th>' + getClientMessage('USERNAME') + '</th><th>' + getClientMessage('USERROLE') + '</th></tr>');
        userListTable.append(tHead);

        var tBody = $('<tbody>');
        for (var Loop = 0; Loop < userList.length; Loop++) {
            var StrUserRole = '';
            if (userList[Loop].Role == 0) {
                StrUserRole = getClientMessage('USERROLEADMIN');
            } else {
                StrUserRole = getClientMessage('USERROLEUSER');
            }
            tBody.append('<tr><td><div class="radio"><label><input type="radio" id="radioUser' + userList[Loop].Id + '" name="optradio" onclick="selectUser(\''+ userList[Loop].Id + '\')"/>&nbsp;' + userList[Loop].Name + '</label></div></td><td>' + StrUserRole + '</td></tr>');
        }
        userListTable.append(tBody);
        $('#usermgmt').append(userListTable);
    }
    $('#usermgmt').append('<div id="usermgmt-ope" style="margin-top:30px;padding-left:30px;padding-right:30px;padding-top:30px;padding-bottom:30px;border-color:#000000;border-style:solid;border-width:2px">');
    $('#usermgmt-ope').append('<div class="form-group"><label for="userName">' + getClientMessage('USERNAME') + '</label><input type="text" class="form-control" id="userName" placeholder="' + getClientMessage('USERNAME') + '"/></div>');
    $('#usermgmt-ope').append('<div class="form-group"><label for="userType">' + getClientMessage('USERROLE') + '</label><select class="form-control" id="userRole"><option>' + getClientMessage('USERROLEADMIN') + '</option><option>' + getClientMessage('USERROLEUSER') + '</option></select></div>');

    var userPwOn = '';
    $('#usermgmt-ope').append($('<div class="form-check"><input class="form-check-input" type="checkbox" id="userPwOn" onClick="clickUserPwOn()" ' + userPwOn + '><label class="form-check-label" for="userPwOn">' + getClientMessage('USER_PASSWORD_ON') + '</label><input type="password" class="form-control" id="userPassword" placeholder="' + getClientMessage('USER_PASSWORD') + '" disabled/></div>'));

    $('#usermgmt').append('<br/>');
    $('#usermgmt').append('<div id="usermgt_msg"/>');
    if (userOpeStatus == 0) {
    } else if (userOpeStatus == 1) {
        displayAlertSuccess('#usermgt_msg', getClientMessage('USEROPECOMPLETED'));
    } else {
        displayAlertDanger('#usermgt_msg', getSvrMsg(responseData['API_OPE_USER']));
    }
    $('#usermgmt').append('<button type="button" id="userBtnAdd" class="btn btn-dark" onclick="updateUser(false)">' + getClientMessage('COMADD') + '</button> ');
    $('#usermgmt').append('<button type="button" id="userBtnUpdate" class="btn btn-dark disabled">' + getClientMessage('COMUPDATE') + '</button> ');
    $('#usermgmt').append('<button type="button" id="userBtnDelete" class="btn btn-dark disabled">' + getClientMessage('COMDELETE') + '</button> ');
    $('#usermgmt').append('<button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('DLG_CLOSE') + '</button>');
    $('#usermgmt').append('<p></p>');
    $('td').css('vertical-align', 'middle');
}

function clickUserPwOn()
{
    if ($('#userPwOn').prop('checked') == true) {
        $('#userPassword').prop('disabled', false);
    } else {
        $('#userPassword').prop('disabled', true);
    }
}

function updateUser(opeFlag) {
    var specifiedUserName = $('#userName').val().replace(/[\n\r]/g, '');
    var specifiedUserRole = $('#userRole').val();
    var specifiedUserPassword = $('#userPassword').val().replace(/[\n\r]/g, '');
    var tmpRole = -1;
    if (specifiedUserRole === getClientMessage('USERROLEADMIN')) {
        tmpRole = 0;
    } else if (specifiedUserRole === getClientMessage('USERROLEUSER')) {
        tmpRole = 1;
    } else {
        closeInputModal();
    }
    if (opeFlag == false) {
        selectedUserId = -1;
    }

    var reqDatDf = {};
    if ($('#userPwOn').prop('checked') == true) {
        reqDatDf = { 'Id': selectedUserId, 'Name': specifiedUserName, 'Role': tmpRole, 'Password': specifiedUserPassword };
    } else {
        reqDatDf = { 'Id': selectedUserId, 'Name': specifiedUserName, 'Role': tmpRole };
    }
    apiCall('POST', '/api/user/', reqDatDf, 'API_OPE_USER', userOpeFinal);
}

function deleteUser() {
    apiCall('DELETE', '/api/user/' + selectedUserId + '/', null, 'API_OPE_USER', userOpeFinal);
}

function userOpeFinal() {
    if (statusCode['API_OPE_USER'] == 200) {
        userOpeStatus = 1;
    } else {
        userOpeStatus = 2;
    }
    
    var contents = [{ method: 'GET', url: '/api/user/?target=all', request: null, keystring: 'API_GET_USERS' }];
    MultiApiCall(contents, displayUser);
}

function selectUser(userId) {
    var userList = getArray(responseData['API_GET_USERS'].Data.User);
    for (loop = 0; loop < userList.length; loop++) {
        if (userList[loop].Id == userId) {
            selectedUserId = userList[loop].Id;
            var roleStr = '';
            if (userList[loop].Role == 0) {
                roleStr = getClientMessage('USERROLEADMIN');
            } else {
                roleStr = getClientMessage('USERROLEUSER');
            }
            $('#userName').val(userList[loop].Name);
            $('#userRole').val(roleStr);
            $('#userPwOn').prop('checked', false);
            $('#userPassword').prop('disabled', true);
            $('#userPassword').val('');
            $('#userBtnUpdate').removeClass('disabled');
            $('#userBtnUpdate').click(function() {updateUser(true);});
            $('#userBtnDelete').removeClass('disabled');
            $('#userBtnDelete').click(function() {deleteUser();});
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

function transDisplayChgPassword() {
    userOpeStatus = 0;
    var contents = [{ method: 'GET', url: '/api/user/', request: null, keystring: 'API_GET_USER' }];
    MultiApiCall(contents, displayChgPassword);
}

function displayChgPassword() {
    var userMgmt = $('<div id="chgpassword">');
    showInputModal(getClientMessage('USER_CHG_PW'), userMgmt);

    if (statusCode['API_GET_USER'] == -1 || statusCode['API_GET_USER'] == 0) {
        displayAlertDanger('#chgpassword', getClientMessage('CONNERR'));
        userMgmt.append('<p><button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('DLG_CLOSE') + '</button></p>');
        return;
    } else if (statusCode['API_GET_USER'] != 200) {
        displayAlertDanger('#chgpassword', getSvrMsg(responseData['API_GET_USER']));
        userMgmt.append('<p><button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('DLG_CLOSE') + '</button></p>');
        return;
    }

    userMgmt.append('<div class="form-group"><label for="currentPw">' + getClientMessage('USER_CHG_PW_CURRENT') + '</label><input type="password" class="form-control" id="currentPw" placeholder="' + getClientMessage('USER_CHG_PW_CURRENT') + '"/></div>');
    userMgmt.append('<div class="form-group"><label for="newPw">' + getClientMessage('USER_CHG_PW_NEW') + '</label><input type="password" class="form-control" id="newPw" placeholder="' + getClientMessage('USER_CHG_PW_NEW') + '"/></div>');
    userMgmt.append('<div class="form-group"><label for="confirmNewPw">' + getClientMessage('USER_CHG_PW_CONFIRM') + '</label><input type="password" class="form-control" id="confirmNewPw" placeholder="' + getClientMessage('USER_CHG_PW_CONFIRM') + '"/></div>');

    userMgmt.append('<br/>');
    userMgmt.append('<div id="chgpassword_msg"/>');

    userMgmt.append('<button type="button" id="chgpassword_update" class="btn btn-dark" onclick="chgPwUpdate()">' + getClientMessage('COMUPDATE') + '</button> ');
    userMgmt.append('<button type="button" id="chgpassword_cancel" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('DLG_CLOSE') + '</button>');
}

function chgPwUpdate() {
    var specifiedCurrentPw = $('#currentPw').val().replace(/[\n\r]/g, '');
    var specifiedNewPw = $('#newPw').val().replace(/[\n\r]/g, '');
    var specifiedConfirmNewPw = $('#confirmNewPw').val().replace(/[\n\r]/g, '');

    if (!isPasswordCorrect(specifiedCurrentPw)) {
        displayAlertDanger('#chgpassword_msg', getClientMessage('USER_PW_WRONG'));
        return;
    }
    if (specifiedNewPw !== specifiedConfirmNewPw) {
        displayAlertDanger('#chgpassword_msg', getClientMessage('USER_NEWPW_WRONG'));
        return;
    }

    var reqDatDf = {};
    reqDatDf = { 'Id': responseData['API_GET_USER'].Data.User.Id, 'Password': specifiedNewPw };
    apiCall('POST', '/api/user/', reqDatDf, 'API_OPE_USER', chgPwFinal);
}

function chgPwFinal() {
    if (statusCode['API_OPE_USER'] == 200) {
        displayAlertSuccess('#chgpassword_msg', getClientMessage('USEROPECOMPLETED'));
        var specifiedNewPw = $('#newPw').val().replace(/[\n\r]/g, '');
        changeLoginPassword(specifiedNewPw);
        setAuthenticationToken(getUserName() + ' ' + specifiedNewPw);
    } else {
        displayAlertDanger('#chgpassword_msg', getSvrMsg(responseData['API_OPE_USER']));
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
