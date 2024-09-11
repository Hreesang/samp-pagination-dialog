/*

	Using the latest version of the open.mp server

*/

// AddPaginationDialogItemStr
CMD:dialog(playerid, params[]){
	new 
		List:items = list_new();

	AddPaginationDialogItemStr(items, @("Index\tColumn 2\tColumn 3\n"));

	for (new i = 0; i < 1000; i++) {
		AddPaginationDialogItemStr(items, str_format("%d\tText %d\t%x\n", i, random(100) + i, random(999999)));
	}

	task_yield(1);

	new response[E_ASYNC_PAGE_DIALOG];
	await_arr(response) ShowPaginationDialogAsync(playerid, PAGE_DIALOG_STYLE_TABLIST_HDRS, "Async Dialog ({page}/{maxPages})", items, "Select", "Close");

	SendClientMessage(playerid, -1, "ShowPaginationDialogAsync response %s, listitem %d", response[E_ASYNC_PAGE_DIALOG_RESPONSE] ? "true" : "false", response[E_ASYNC_PAGE_DIALOG_LISTITEM]);
}

// AddPaginationDialogItem
CMD:dialog(playerid, params[]){
	new 
		List:items = list_new(),
		temp[16]
	;
	
	for (new i = 0; i < 200; i++) {
		format(temp, sizeof(temp), "List: index %d", i);
		AddPaginationDialogItem(items, temp);
	}

	task_yield(1);

	new response[E_ASYNC_PAGE_DIALOG];
	await_arr(response) ShowPaginationDialogAsync(playerid, PAGE_DIALOG_STYLE_LIST, "Async Dialog ({page}/{maxPages})", items, "Select", "Close");

	SendClientMessage(playerid, -1, "ShowPaginationDialogAsync response %s, listitem %d", response[E_ASYNC_PAGE_DIALOG_RESPONSE] ? "true" : "false", response[E_ASYNC_PAGE_DIALOG_LISTITEM]);
}

// skin list
CMD:skins(playerid, params[]){
	new 
		List:items = list_new();

	for (new i = 0; i < 312; i++) {
		AddPaginationDialogItemStr(items, str_format("Skin %d\n", i));
	}

	task_yield(1);

	new response[E_ASYNC_PAGE_DIALOG];
	await_arr(response) ShowPaginationDialogAsync(playerid, PAGE_DIALOG_STYLE_LIST, "Skin List ({page}/{maxPages})", items, "Select", "Close");

	if(!response[E_ASYNC_PAGE_DIALOG_RESPONSE])
		return;

	SetPlayerSkin(playerid, response[E_ASYNC_PAGE_DIALOG_LISTITEM]);

	SendClientMessage(playerid, -1, "skin changed to %d.", response[E_ASYNC_PAGE_DIALOG_LISTITEM]);
}

// weapon list
CMD:weapons(playerid, params[]){
	new 
		List:items = list_new(),
		weapon_name[32]
	;

	AddPaginationDialogItemStr(items, @("Name\tID\tSlot\n"));

	for (new i = 0; i < 47; i++) {
		GetWeaponName(_:i, weapon_name, sizeof(weapon_name));
		AddPaginationDialogItemStr(items, str_format("%s\t%d\t%d\n", weapon_name, i, _:GetWeaponSlot(i), ));
	}

	task_yield(1);

	new response[E_ASYNC_PAGE_DIALOG];
	await_arr(response) ShowPaginationDialogAsync(playerid, PAGE_DIALOG_STYLE_TABLIST_HDRS, "Weapon List ({page}/{maxPages})", items, "Select", "Close");

	if(!response[E_ASYNC_PAGE_DIALOG_RESPONSE])
		return;

	GivePlayerWeapon(playerid, response[E_ASYNC_PAGE_DIALOG_LISTITEM], 100);

	GetWeaponName(_:response[E_ASYNC_PAGE_DIALOG_LISTITEM], weapon_name);
	SendClientMessage(playerid, -1, "Weapon %s (%d)", weapon_name, response[E_ASYNC_PAGE_DIALOG_LISTITEM]);
}