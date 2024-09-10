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

Have a look at `test.pwn` for more detailed usage information.

## Testing

<!--
Depending on whether your package is tested via in-game "demo tests" or
y_testing unit-tests, you should indicate to readers what to expect below here.
-->

To test, simply build and run the package:

```bash
sampctl build
sampctl run
```

## APIs

TBA

## Credits

- [Hreesang](https://github.com/Hreesang), the library creator
