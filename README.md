# samp-pagination-dialog

[![sampctl](https://img.shields.io/badge/sampctl-samp--pagination--dialog-2f2f2f.svg?style=for-the-badge)](https://github.com/Hreesang/samp-pagination-dialog)

SA:MP or open.mp's dialog list, currently, limits the items that you want to show to your players by the capped string size which is 4096. Yet, this library breaks that limit. If the items string size in total is more than 4096, the items will be broken into pages. It's also possible for you to specify how many items you want to show per page, not always depending on the 4096 string size limit. If the size of items you want to show per page is more than 4096 string size, yet it will still be capped into 4096 string size per page.

## Installation

Download the library or simply use sampctl to do so:

```bash
sampctl package install Hreesang/samp-pagination-dialog
```

Include in your code and begin using the library:

```pawn
#include <PawnPlus>
#include <pagination_dialog>
```

To be noted that you need [PawnPlus](https://github.com/IS4Code/PawnPlus) to make this library works, so include it before including this library.

## Usage

```pawn
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

public OnPaginationDialogResponse(playerid, dialogid, bool:response, listitem)
{
	new string[144];
	format(string, sizeof string, "[OnPaginationDialogResponse] playerid: %i, dialogid: %i, response: %s, listitem: %i", playerid, dialogid, response ? "true" : "false", listitem);
	SendClientMessage(playerid, -1, string);
	return 1;
}
```

Have a look at [test.pwn](./test.pwn) for more detailed usage information.

## Testing

To test, simply build and run the package:

```bash
sampctl build
sampctl run
```

## Declarations

| Declaration                     | Value                           | Editable? | Description                                                                         |
| ------------------------------- | ------------------------------- | --------- | ----------------------------------------------------------------------------------- |
| PAGINATION_DIALOG_ID            | 1847                            | Yes       | Index used to reference the native's dialog.                                        |
| PAGINATION_DIALOG_PP_ID         | 0x502B                          | Yes       | Reserved dialog ID for the asynchronous feature.                                    |
| DEFAULT_PAGINATION_DIALOG_ITEMS | -1                              | Yes       | Page breaks depends on 4096 string size if the max items value is using this value. |
| MAX_PAGINATION_DIALOG_ITEMS     | DEFAULT_PAGINATION_DIALOG_ITEMS | Yes       | Max items to be shown per page, depending on 4096 string size is the default.       |
| MAX_PAGINATION_DIALOG_CAPTION   | 64                              | Yes       | Max string size for the dialog caption.                                             |
| MAX_PAGINATION_DIALOG_BUTTON    | 64                              | Yes       | Max string size for the dialog buttons.                                             |
| PAGE_DIALOG_STYLE_LIST          | DIALOG_STYLE_LIST               | No        | Pagination dialog style                                                             |
| PAGE_DIALOG_STYLE_TABLIST       | DIALOG_STYLE_TABLIST            | No        | Pagination dialog style                                                             |
| PAGE_DIALOG_STYLE_TABLIST_HDRS  | DIALOG_STYLE_TABLIST_HDRS       | No        | Pagination dialog style                                                             |

## APIs

### pagination_dialog.inc

```pawn
public OnPaginationDialogResponse(playerid, dialogid, bool:response, listitem)
```

> **Parameters:**
>
> - `playerid` Player who received the dialog response.
> - `dialogid` Index of pagination dialog that's triggered by the player.
> - `bool:response` Boolean response of the dialog: `true` and `false`. It will be `false` if player closes the dialog on next or previous button/item.
> - `listitem` Index of item the player selected. `-1` if player closed the dialog on next or previous button.

> **Return Values:**
>
> - `any number` Nothing happens.

```pawn
stock bool:ShowPaginationDialog(playerid, dialogid, PAGE_DIALOG_STYLE:style, const caption[], List:items, const button1[], const button2[] = "", const nextButton[] = ">>>", const prevButton[] = "<<<", page = 0, maxItems = MAX_PAGINATION_DIALOG_ITEMS)
```

Shows a pagination dialog to specified player.

> **Parameters:**
>
> - `playerid` Player who will be shown the dialog.
> - `dialogid` Index of pagination dialog that will be triggered by the player.
> - `style` Pagination dialog style.
> - `caption` Caption for the pagination dialog. Use `{page}` to display the current page and `{maxPages}` for the max pages, see [test.pwn](./test.pwn#48).
> - `List:items` Items of the dialog that's added by `AddPaginationDialogItem(Str)`.
> - `const button1[]` Left button of the dialog response.
> - `const button2[]` Right button of the dialog response.
> - `const nextButton[]` Button to move to the next page, default is `>>>`.
> - `const prevButton[]` Button to move to the previous page, default is `<<<`.
> - `page` Page number to be opened at first, default is `0`.
> - `maxItems` Max items per page, default is `MAX_PAGINATION_DIALOG_ITEMS`.

> **Return Values:**
>
> - `true` if success.
> - `false` if not success: player not connected or invalid items.

```pawn
stock AddPaginationDialogItem(List:items, const string[])
stock AddPaginationDialogItemStr(List:items, ConstStringTag:str)
```

Adds an item to the list of items for a pagination dialog.

> **Parameters:**
>
> - `List:items` List of the items created by `list_new`.
> - `string` or `ConstStringTag:str` Item string to be put into the list. It supports both plain and PawnPlus' string.

```pawn
stock bool:OpenPaginationDialogPage(playerid, page)
```

Navigates to a page from the specified player's current opened pagination dialog.

> **Parameters:**
>
> - `playerid` Player that will do the navigation.
> - `page` Page number to navigate. It's zero-indexed.

> **Return Values:**
>
> - `true` if success.
> - `false` if player doesn't have any active pagination dialog.

```pawn
stock bool:ClosePaginationDialog(playerid)
```

Closes the specified player's current opened pagination dialog.

> **Parameters:**
>
> - `playerid` Player that will close the dialog.

> **Return Values:**
>
> - `true` if success
> - `false` if player doesn't have any active pagination dialog.

### pagination_dialog_async.inc

```pawn
stock Task:ShowPaginationDialogAsync(playerid, PAGE_DIALOG_STYLE:style, const caption[], List:items, const button1[], const button2[] = "", const nextButton[] = ">>>", const prevButton[] = "<<<", page = 0, maxItems = MAX_PAGINATION_DIALOG_ITEMS)

main()
{
	...

	new Task:t = ShowPaginationDialogAsync(playerid, PAGE_DIALOG_STYLE_TABLIST_HDRS, "Async Dialog ({page}/{maxPages})", items, "Asu", .maxItems = maxItems);
	new response[E_ASYNC_PAGE_DIALOG];

	yield(1);
	await_arr(t, response);

	new string[144];
	format(string, sizeof string, "[Pagination Dialog Async] response: %s, listitem: %i", response[E_ASYNC_PAGE_DIALOG_RESPONSE] ? "true" : "false", response[E_ASYNC_PAGE_DIALOG_LISTITEM]);
	SendClientMessage(playerid, -1, string);
}
```

Shows a pagination dialog to specified player, but this one has PawnPlus' task support.

> **Parameters:**
>
> - `playerid` Player who will be shown the dialog.
> - `style` Pagination dialog style.
> - `caption` Caption for the pagination dialog. Use `{page}` to display the current page and `{maxPages}` for the max pages, see [test.pwn](./test.pwn#48).
> - `List:items` Items of the dialog that's added by `AddPaginationDialogItem(Str)`.
> - `const button1[]` Left button of the dialog response.
> - `const button2[]` Right button of the dialog response.
> - `const nextButton[]` Button to move to the next page, default is `>>>`.
> - `const prevButton[]` Button to move to the previous page, default is `<<<`.
> - `page` Page number to be opened at first, default is `0`.
> - `maxItems` Max items per page, default is `MAX_PAGINATION_DIALOG_ITEMS`.

> **Return Values:**
>
> - `true` if success.
> - `false` if not success: player not connected or invalid items.

## Credits

- [Hreesang](https://github.com/Hreesang) for making the library.
