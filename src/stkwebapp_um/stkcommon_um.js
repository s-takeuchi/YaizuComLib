
{
    //
    // User information
    //
    addClientMessage('STKCOMMONUG_USERMGMT', {'en':'User Management', 'ja':'ユーザー管理'});
    addClientMessage('STKCOMMONUG_NOUSEREXIST', {'en':'<p>No user exists</p>', 'ja':'<p>ユーザーは存在しません</p>'});
    addClientMessage('STKCOMMONUG_USERNAME', {'en':'User Name', 'ja':'ユーザー名'});
    addClientMessage('STKCOMMONUG_USERROLE', {'en':'User Role', 'ja':'ユーザーロール'});
    addClientMessage('STKCOMMONUG_USERROLEADMIN', {'en':'Administrator', 'ja':'管理者'});
    addClientMessage('STKCOMMONUG_USERROLEUSER', {'en':'General User', 'ja':'一般ユーザー'});
    addClientMessage('STKCOMMONUG_USEROPECOMPLETED', {'en':'The operation has completed.', 'ja':'操作が完了しました。'});
    addClientMessage('STKCOMMONUG_USER_PASSWORD_ON', {'en':'Configure password', 'ja':'パスワードを設定する'});
    addClientMessage('STKCOMMONUG_USER_PASSWORD', {'en':'Password', 'ja':'パスワード'});
    addClientMessage('STKCOMMONUG_USER_CHG_PW', {'en':'Change Password', 'ja':'パスワードの変更'});
    addClientMessage('STKCOMMONUG_USER_CHG_PW_CURRENT', {'en':'Current Password', 'ja':'現在のパスワード'});
    addClientMessage('STKCOMMONUG_USER_CHG_PW_NEW', {'en':'New Password', 'ja':'新しいパスワード'});
    addClientMessage('STKCOMMONUG_USER_CHG_PW_CONFIRM', {'en':'Confirm New Password', 'ja':'新しいパスワード(確認用)'});
    addClientMessage('STKCOMMONUG_USER_PW_WRONG', {'en':'The specified password is not correct.', 'ja':'指定されたパスワードが不正です。'});
    addClientMessage('STKCOMMONUG_USER_NEWPW_WRONG', {'en':'The specified "New Password" and "Confirm New Password" are not matched.', 'ja':'"新しいパスワード"と"新しいパスワード(確認用)"が不一致です。'});

    //
    // Logging information
    //
    addClientMessage('STKCOMMONUG_LOGINFO', {'en':'Operation log', 'ja':'操作ログ'});
    addClientMessage('STKCOMMONUG_LOGEVENTTIME', {'en':'Event Occurrence Time', 'ja':'イベント発生時刻'});
    addClientMessage('STKCOMMONUG_LOGEVENTUSER', {'en':'User', 'ja':'ユーザー'});
    addClientMessage('STKCOMMONUG_LOGEVENTUSER_SYSTEM', {'en':'System', 'ja':'System'});
    addClientMessage('STKCOMMONUG_LOGEVENT', {'en':'Event', 'ja':'イベント'});
    addClientMessage('STKCOMMONUG_NOLOGINFO', {'en':'<p>No log information</p>', 'ja':'<p>ログはありません</p>'});

    //
    // Others
    //
    addClientMessage('STKCOMMONUG_CONNERR', {
        'en':'Connection with REST API service failed. This may be caused by one of the following issues:<br>(1) REST API service cannot be started.<br>(2) REST API service is not registered as a firewall exception.<br>(3) The host name and/or port number you specified are/is invalid.<br>(4) A timeout has occurred when waiting for data from REST API server.<br>',
        'ja':'REST APIサービスとの通信が失敗しました。次の要因が考えられます。<br>(1) REST APIサービスが開始されていない。<br>(2) REST APIサービスがファイアウォールに例外登録されていない。<br>(3) 指定した接続先ホスト名およびポート番号が不正。<br>(4) REST APIサーバからのデータ取得中にタイムアウトが発生した。<br>'
    });
    addClientMessage('STKCOMMONUG_DLG_CLOSE', {'en':'Close', 'ja':'閉じる'});
    addClientMessage('STKCOMMONUG_COMADD', {'en':'Add', 'ja':'追加'});
    addClientMessage('STKCOMMONUG_COMUPDATE', {'en':'Update', 'ja':'更新'});
    addClientMessage('STKCOMMONUG_COMDELETE', {'en':'Delete', 'ja':'削除'});
}

{
    // Selected user information on user management page
    let selectedUserId = -1;

    // User operation result
    let userOpeStatus = 0;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    function transDisplayUser() {
        userOpeStatus = 0;
        let contents = [{ method: 'GET', url: '/api/user/?target=all', request: null, keystring: 'API_GET_USERS' }];
        MultiApiCall(contents, displayUser);
    }

    function displayUser() {
        let userMgmt = $('<div id="usermgmt">');
        showInputModal(getClientMessage('STKCOMMONUG_USERMGMT'), userMgmt);

        if (statusCode['API_GET_USERS'] == -1 || statusCode['API_GET_USERS'] == 0) {
            displayAlertDanger('#usermgmt', getClientMessage('STKCOMMONUG_CONNERR'));
            $('#usermgmt').append('<p><button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('STKCOMMONUG_DLG_CLOSE') + '</button></p>');
            return;
        } else if (statusCode['API_GET_USERS'] != 200) {
            displayAlertDanger('#usermgmt', getSvrMsg(responseData['API_GET_USERS']));
            $('#usermgmt').append('<p><button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('STKCOMMONUG_DLG_CLOSE') + '</button></p>');
            return;
        }
        let userList = getArray(responseData['API_GET_USERS'].Data.User);
        if (userList == null) {
            $('#usermgmt').append(getClientMessage('STKCOMMONUG_NOUSEREXIST'));
        }

        selectedUserId = -1;

        if (responseData['API_GET_USERS'].Data.User !== undefined) {
            let userListTable = $('<table>');
            userListTable.addClass('table table-striped');

            let tHead = $('<thead class="thead-dark">');
            tHead.append('<tr><th>' + getClientMessage('STKCOMMONUG_USERNAME') + '</th><th>' + getClientMessage('STKCOMMONUG_USERROLE') + '</th></tr>');
            userListTable.append(tHead);

            let tBody = $('<tbody>');
            for (let Loop = 0; Loop < userList.length; Loop++) {
                let StrUserRole = '';
                if (userList[Loop].Role == 0) {
                    StrUserRole = getClientMessage('STKCOMMONUG_USERROLEADMIN');
                } else {
                    StrUserRole = getClientMessage('STKCOMMONUG_USERROLEUSER');
                }
                tBody.append('<tr><td><div class="radio"><label><input type="radio" id="radioUser' + userList[Loop].Id + '" name="optradio" onclick="selectUser(\''+ userList[Loop].Id + '\')"/>&nbsp;' + userList[Loop].Name + '</label></div></td><td>' + StrUserRole + '</td></tr>');
            }
            userListTable.append(tBody);
            $('#usermgmt').append(userListTable);
        }
        $('#usermgmt').append('<div id="usermgmt-ope" style="margin-top:30px;padding-left:30px;padding-right:30px;padding-top:30px;padding-bottom:30px;border-color:#000000;border-style:solid;border-width:2px">');
        $('#usermgmt-ope').append('<div class="form-group"><label for="userName">' + getClientMessage('STKCOMMONUG_USERNAME') + '</label><input type="text" class="form-control" id="userName" placeholder="' + getClientMessage('STKCOMMONUG_USERNAME') + '"/></div>');
        $('#usermgmt-ope').append('<div class="form-group"><label for="userType">' + getClientMessage('STKCOMMONUG_USERROLE') + '</label><select class="form-control" id="userRole"><option>' + getClientMessage('STKCOMMONUG_USERROLEADMIN') + '</option><option>' + getClientMessage('STKCOMMONUG_USERROLEUSER') + '</option></select></div>');

        let userPwOn = '';
        $('#usermgmt-ope').append($('<div class="form-check"><input class="form-check-input" type="checkbox" id="userPwOn" onClick="clickUserPwOn()" ' + userPwOn + '><label class="form-check-label" for="userPwOn">' + getClientMessage('STKCOMMONUG_USER_PASSWORD_ON') + '</label><input type="password" class="form-control" id="userPassword" placeholder="' + getClientMessage('STKCOMMONUG_USER_PASSWORD') + '" disabled/></div>'));

        $('#usermgmt').append('<br/>');
        $('#usermgmt').append('<div id="usermgt_msg"/>');
        if (userOpeStatus == 0) {
        } else if (userOpeStatus == 1) {
            displayAlertSuccess('#usermgt_msg', getClientMessage('STKCOMMONUG_USEROPECOMPLETED'));
        } else {
            displayAlertDanger('#usermgt_msg', getSvrMsg(responseData['API_OPE_USER']));
        }
        $('#usermgmt').append('<button type="button" id="userBtnAdd" class="btn btn-dark" onclick="updateUser(false)">' + getClientMessage('STKCOMMONUG_COMADD') + '</button> ');
        $('#usermgmt').append('<button type="button" id="userBtnUpdate" class="btn btn-dark disabled">' + getClientMessage('STKCOMMONUG_COMUPDATE') + '</button> ');
        $('#usermgmt').append('<button type="button" id="userBtnDelete" class="btn btn-dark disabled">' + getClientMessage('STKCOMMONUG_COMDELETE') + '</button> ');
        $('#usermgmt').append('<button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('STKCOMMONUG_DLG_CLOSE') + '</button>');
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
        let specifiedUserName = $('#userName').val().replace(/[\n\r]/g, '');
        let specifiedUserRole = $('#userRole').val();
        let specifiedUserPassword = $('#userPassword').val().replace(/[\n\r]/g, '');
        let tmpRole = -1;
        if (specifiedUserRole === getClientMessage('STKCOMMONUG_USERROLEADMIN')) {
            tmpRole = 0;
        } else if (specifiedUserRole === getClientMessage('STKCOMMONUG_USERROLEUSER')) {
            tmpRole = 1;
        } else {
            closeInputModal();
        }
        if (opeFlag == false) {
            selectedUserId = -1;
        }

        let reqDatDf = {};
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
    
        let contents = [{ method: 'GET', url: '/api/user/?target=all', request: null, keystring: 'API_GET_USERS' }];
        MultiApiCall(contents, displayUser);
    }

    function selectUser(userId) {
        let userList = getArray(responseData['API_GET_USERS'].Data.User);
        for (loop = 0; loop < userList.length; loop++) {
            if (userList[loop].Id == userId) {
                selectedUserId = userList[loop].Id;
                let roleStr = '';
                if (userList[loop].Role == 0) {
                    roleStr = getClientMessage('STKCOMMONUG_USERROLEADMIN');
                } else {
                    roleStr = getClientMessage('STKCOMMONUG_USERROLEUSER');
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
        let contents = [{ method: 'GET', url: '/api/user/', request: null, keystring: 'API_GET_USER' }];
        MultiApiCall(contents, displayChgPassword);
    }

    function displayChgPassword() {
        let userMgmt = $('<div id="chgpassword">');
        showInputModal(getClientMessage('STKCOMMONUG_USER_CHG_PW'), userMgmt);

        if (statusCode['API_GET_USER'] == -1 || statusCode['API_GET_USER'] == 0) {
            displayAlertDanger('#chgpassword', getClientMessage('STKCOMMONUG_CONNERR'));
            userMgmt.append('<p><button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('STKCOMMONUG_DLG_CLOSE') + '</button></p>');
            return;
        } else if (statusCode['API_GET_USER'] != 200) {
            displayAlertDanger('#chgpassword', getSvrMsg(responseData['API_GET_USER']));
            userMgmt.append('<p><button type="button" id="closeOdbcConfig" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('STKCOMMONUG_DLG_CLOSE') + '</button></p>');
            return;
        }

        userMgmt.append('<div class="form-group"><label for="currentPw">' + getClientMessage('STKCOMMONUG_USER_CHG_PW_CURRENT') + '</label><input type="password" class="form-control" id="currentPw" placeholder="' + getClientMessage('STKCOMMONUG_USER_CHG_PW_CURRENT') + '"/></div>');
        userMgmt.append('<div class="form-group"><label for="newPw">' + getClientMessage('STKCOMMONUG_USER_CHG_PW_NEW') + '</label><input type="password" class="form-control" id="newPw" placeholder="' + getClientMessage('STKCOMMONUG_USER_CHG_PW_NEW') + '"/></div>');
        userMgmt.append('<div class="form-group"><label for="confirmNewPw">' + getClientMessage('STKCOMMONUG_USER_CHG_PW_CONFIRM') + '</label><input type="password" class="form-control" id="confirmNewPw" placeholder="' + getClientMessage('STKCOMMONUG_USER_CHG_PW_CONFIRM') + '"/></div>');

        userMgmt.append('<br/>');
        userMgmt.append('<div id="chgpassword_msg"/>');

        userMgmt.append('<button type="button" id="chgpassword_update" class="btn btn-dark" onclick="chgPwUpdate()">' + getClientMessage('STKCOMMONUG_COMUPDATE') + '</button> ');
        userMgmt.append('<button type="button" id="chgpassword_cancel" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('STKCOMMONUG_DLG_CLOSE') + '</button>');
    }

    function chgPwUpdate() {
        let specifiedCurrentPw = $('#currentPw').val().replace(/[\n\r]/g, '');
        let specifiedNewPw = $('#newPw').val().replace(/[\n\r]/g, '');
        let specifiedConfirmNewPw = $('#confirmNewPw').val().replace(/[\n\r]/g, '');

        if (!isPasswordCorrect(specifiedCurrentPw)) {
            displayAlertDanger('#chgpassword_msg', getClientMessage('STKCOMMONUG_USER_PW_WRONG'));
            return;
        }
        if (specifiedNewPw !== specifiedConfirmNewPw) {
            displayAlertDanger('#chgpassword_msg', getClientMessage('STKCOMMONUG_USER_NEWPW_WRONG'));
            return;
        }

        let reqDatDf = {};
        reqDatDf = { 'Id': responseData['API_GET_USER'].Data.User.Id, 'Password': specifiedNewPw };
        apiCall('POST', '/api/user/', reqDatDf, 'API_OPE_USER', chgPwFinal);
    }

    function chgPwFinal() {
        if (statusCode['API_OPE_USER'] == 200) {
            displayAlertSuccess('#chgpassword_msg', getClientMessage('STKCOMMONUG_USEROPECOMPLETED'));
            let specifiedNewPw = $('#newPw').val().replace(/[\n\r]/g, '');
            changeLoginPassword(specifiedNewPw);
            setAuthenticationToken(getUserName() + ' ' + specifiedNewPw);
        } else {
            displayAlertDanger('#chgpassword_msg', getSvrMsg(responseData['API_OPE_USER']));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
{
    function transDisplayLogInfo() {
        let contents = [];
        if (statusCode['API_GET_USER'] == 200 && responseData['API_GET_USER'].Data.User.Role == 0) {
            contents = [
                { method: 'GET', url: '/api/user/?target=all', request: null, keystring: 'API_GET_USERS' },
                { method: 'GET', url: '/api/logs/', request: null, keystring: 'API_GET_LOGS' }
            ];
        } else {
            contents = [
                { method: 'GET', url: '/api/logs/', request: null, keystring: 'API_GET_LOGS' }
            ];
        }
        MultiApiCall(contents, displayLogInfo);
    }

    function displayLogInfo() {
        let logInfo = $('<div id="loginfodlg">');
        showInputModal(getClientMessage('STKCOMMONUG_LOGINFO'), logInfo);

        if (statusCode['API_GET_LOGS'] == -1 || statusCode['API_GET_LOGS'] == 0) {
            displayAlertDanger('#loginfodlg', getClientMessage('CSTKCOMMONUG_CONNERR'));
            return;
        } else if (statusCode['API_GET_LOGS'] != 200) {
            displayAlertDanger('#loginfodlg', getSvrMsg(responseData['API_GET_LOGS']));
            return;
        }

        let Log = getArray(responseData['API_GET_LOGS'].Data.Log);
        if (Log == null) {
            $('#loginfodlg').append(getClientMessage('STKCOMMONUG_NOLOGINFO'));
            return;
        }

        let userList = [];
        if (responseData['API_GET_USERS'] === undefined || responseData['API_GET_USERS'].Data === undefined || responseData['API_GET_USERS'].Data.User === undefined) {
            userList.push(responseData['API_GET_USER'].Data.User);
        } else {
            userList = getArray(responseData['API_GET_USERS'].Data.User);
        }

        let logData = $('<table>');
        logData.addClass('table table-striped');

        let tHead = $('<thead class="thead-dark">');
        tHead.append('<tr><th>' + getClientMessage('STKCOMMONUG_LOGEVENTTIME') + '</th><th class="d-none d-sm-table-cell">' + getClientMessage('STKCOMMONUG_LOGEVENTUSER') + '</th><th>' + getClientMessage('STKCOMMONUG_LOGEVENT') + '</th></tr>');
        logData.append(tHead);

        let tBody = $('<tbody>');
        for (let Loop = 0; Loop < Log.length; Loop++) {
            let userName = "";
            for (let loopUser = 0; loopUser < userList.length; loopUser++) {
                if (Log[Loop].UserId == userList[loopUser].Id) {
                    userName = userList[loopUser].Name;
                } else if (Log[Loop].UserId == -1) {
                    userName = getClientMessage('STKCOMMONUG_LOGEVENTUSER_SYSTEM');
                }
            }
            let dispTimeStr = "";
            if (/^[0-9a-f]{16}$/i.test(Log[Loop].Time)) {
                let dispTimeInt = parseInt(Log[Loop].Time, 16);
                let dispTimeDate = new Date(dispTimeInt * 1000);
                dispTimeStr = dispTimeDate.toString()
            } else {
                dispTimeStr = Log[Loop].Time;
            }
            if (getClientLanguage() == 1) {
                tBody.append('<tr><td>' + dispTimeStr + '</td><td class="d-none d-sm-table-cell">' + userName + '</td><td>' + Log[Loop].MsgJa + '</td></tr>');
            } else {
                tBody.append('<tr><td>' + dispTimeStr + '</td><td class="d-none d-sm-table-cell">' + userName + '</td><td>' + Log[Loop].MsgEn + '</td></tr>');
            }
        }
        logData.append(tBody);
        $('#loginfodlg').append(logData);
        $('#loginfodlg').append('<button type="button" id="loginfodlg_cancel" class="btn btn-dark" onclick="closeInputModal()">' + getClientMessage('STKCOMMONUG_DLG_CLOSE') + '</button>');

        $('td').css('vertical-align', 'middle');
    }
}
