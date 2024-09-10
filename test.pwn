#include <a_samp>
#include <crashdetect>
#include <sscanf2>
#include <YSI_Visual\y_commands>
#include <test-boilerplate>
#include "pagination_dialog"
#include "pagination_dialog_async"

static const dialogItems[][] = {
	"Hi, I'm Hreesang. Who are you?",
	"Old unsatiable our now but considered travelling impression.",
	"Offices parties lasting outward nothing age few resolve.",
	"Impression to discretion understood to we interested he excellence.",
	"Woody equal ask saw sir weeks aware decay.",
	"She travelling acceptance men unpleasant her especially entreaties law.",
	"Behaviour we improving at something to.",
	"Evil true high lady roof men had open.",
	"Far concluded not his something extremity."
};

main() 
{
	print("Pagination Dialog by Hreesang");
}

@cmd() dialog(const playerid, const params[], const help)
{
	#pragma unused help

	new totalItems, maxItems;
	if (sscanf(params, "iI(-1)", totalItems, maxItems)) {
		return SendClientMessage(playerid, -1, "Usage: /dialog [total items] [opt: max items per page]");
	}

	if (totalItems < 0) {
		return SendClientMessage(playerid, -1, "Total items should be one or more.");
	}

	if (maxItems != DEFAULT_PAGINATION_DIALOG_ITEMS && !(1 <= totalItems)) {
		return SendClientMessage(playerid, -1, "Max items should be one or more.");
	}

	new List:items = list_new();
	for (new i = 0; i < totalItems; i++) {
		new idx = random(sizeof dialogItems);
		AddPaginationDialogItem(items, dialogItems[idx]);
	}

	ShowPaginationDialog(playerid, random(1000), PAGE_DIALOG_STYLE_LIST, "Dialog ({page}/{maxPages})", items, "Asu", .maxItems = maxItems);
	return 1;
}

@cmd() dialogtablist(const playerid, const params[], const help)
{
	#pragma unused help

	new totalItems, maxItems;
	if (sscanf(params, "iI(-1)", totalItems, maxItems)) {
		return SendClientMessage(playerid, -1, "Usage: /dialogtablist [total items] [opt: max items per page]");
	}

	if (totalItems < 0) {
		return SendClientMessage(playerid, -1, "Total items should be one or more.");
	}

	if (maxItems != DEFAULT_PAGINATION_DIALOG_ITEMS && !(1 <= totalItems)) {
		return SendClientMessage(playerid, -1, "Max items should be one or more.");
	}

	new List:items = list_new();
	for (new i = 0; i < totalItems; i++) {
		new idx = random(sizeof dialogItems);
		AddPaginationDialogItemStr(items, str_format("%s\tA tab", dialogItems[idx]));
	}

	ShowPaginationDialog(playerid, random(1000), PAGE_DIALOG_STYLE_TABLIST, "Dialog Tablist ({page}/{maxPages})", items, "Asu", .maxItems = maxItems);
	return 1;
}

@cmd() dialogheaders(const playerid, const params[], const help)
{
	#pragma unused help

	new totalItems, maxItems;
	if (sscanf(params, "iI(-1)", totalItems, maxItems)) {
		return SendClientMessage(playerid, -1, "Usage: /dialogheaders [total items] [opt: max items per page]");
	}

	if (totalItems < 0) {
		return SendClientMessage(playerid, -1, "Total items should be one or more.");
	}

	if (maxItems != DEFAULT_PAGINATION_DIALOG_ITEMS && !(1 <= totalItems)) {
		return SendClientMessage(playerid, -1, "Max items should be one or more.");
	}

	new List:items = list_new();
	AddPaginationDialogItem(items, "Text\tWoah?");
	for (new i = 0; i < totalItems; i++) {
		new idx = random(sizeof dialogItems);
		AddPaginationDialogItemStr(items, str_format("%s\tTrue", dialogItems[idx]));
	}

	ShowPaginationDialog(playerid, random(1000), PAGE_DIALOG_STYLE_TABLIST_HDRS, "Dialog Tablist Headers ({page}/{maxPages})", items, "Asu", .maxItems = maxItems);
	return 1;
}

@cmd() asyncdialog(playerid, const params[], help)
{
	#pragma unused help

	new totalItems, maxItems;
	if (sscanf(params, "iI(-1)", totalItems, maxItems)) {
		return SendClientMessage(playerid, -1, "Usage: /asyncdialog [total items] [opt: max items per page]");
	}

	if (totalItems < 0) {
		return SendClientMessage(playerid, -1, "Total items should be one or more.");
	}

	if (maxItems != DEFAULT_PAGINATION_DIALOG_ITEMS && !(1 <= totalItems)) {
		return SendClientMessage(playerid, -1, "Max items should be one or more.");
	}

	new List:items = list_new();
	AddPaginationDialogItem(items, "Text\tWoah?");
	for (new i = 0; i < totalItems; i++) {
		new idx = random(sizeof dialogItems);
		AddPaginationDialogItemStr(items, str_format("%s\tTrue", dialogItems[idx]));
	}

	new Task:t = ShowPaginationDialogAsync(playerid, PAGE_DIALOG_STYLE_TABLIST_HDRS, "Async Dialog ({page}/{maxPages})", items, "Asu", .maxItems = maxItems);
	new response[E_ASYNC_PAGE_DIALOG];
	
	task_yield(1);
	task_await_arr(t, response);

	new string[144];
	format(string, sizeof string, "[Pagination Dialog Async] response: %s, listitem: %i", response[E_ASYNC_PAGE_DIALOG_RESPONSE] ? "true" : "false", response[E_ASYNC_PAGE_DIALOG_LISTITEM]);
	SendClientMessage(playerid, -1, string);

	return 1;
}

public OnPaginationDialogResponse(playerid, dialogid, bool:response, listitem)
{
	new string[144];
	format(string, sizeof string, "[OnPaginationDialogResponse] playerid: %i, dialogid: %i, response: %s, listitem: %i", playerid, dialogid, response ? "true" : "false", listitem);
	SendClientMessage(playerid, -1, string);
	return 1;
}
