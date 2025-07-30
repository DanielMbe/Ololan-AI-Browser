"use strict";
let selectedBookmark = null;
let selectedFolder = "Bookmark bar";
let sourceFolder = "";
let copiedBookmark = "";
let copiedBookmarkIcon = null;
let cutedBookmark = "";
let isEditing = false;
let onGroupSelection = false;
let groupSelectionCuted = false;
let groupSelectionCopied = false;
let itemSelected = 0;
let selectedItems = [];
let bookmarksCore = null;
let addingBookmark = false;
let undoDeletion = false;
let undoID = -1;
let folderOnDeletion = null;
let bookmarkOnDeletion = null;
let bookmarkListOnDeletion = [];
let activeFolder = null;
let onSearching = false;
let onClearFolder = false;
let folderToClear = "";

function setupConnector() {
    new QWebChannel(qt.webChannelTransport, function(ololanChannel) {
        bookmarksCore = ololanChannel.objects.bookmarksManager;
    });
}

function loadBookmarkItem(title, url, icon, folder, index) {
    if (selectedFolder.localeCompare(folder) == 0) {
        let bookmark = createBookmark("add");
        bookmark.childNodes.item(1).childNodes.item(0).value = title;
        bookmark.childNodes.item(2).childNodes.item(0).value = url;
        bookmark.childNodes.item(1).childNodes.item(0).title = title;
        bookmark.childNodes.item(2).childNodes.item(0).title = url;
        bookmark.childNodes.item(0).src = icon;
        bookmark.dataset.bookmarkID = index;
        bookmark.dataset.onSelection = "false";
    }
}

function loadSearchResults(title, url, icon, folder, index) {
    let bookmark = createBookmark("search");
    bookmark.childNodes.item(1).childNodes.item(0).value = title;
    bookmark.childNodes.item(2).childNodes.item(0).value = url;
    bookmark.childNodes.item(1).childNodes.item(0).title = title;
    bookmark.childNodes.item(2).childNodes.item(0).title = url;
    bookmark.childNodes.item(0).src = icon;
    bookmark.dataset.bookmarkID = index;
    bookmark.dataset.onSelection = "false";
    bookmark.dataset.bookmarkFolder = folder;
}

function selectFolder(clickEvent) {
    let folderButton = clickEvent.target;
    let folderClassName = folderButton.className.trim();
    let folderName = "";

    if (onSearching) {
        onSearching = false;
    }

    if (isEditing) {
        cancel();
    }

    if (onGroupSelection) {
        markBookmark();
    }

    if ((folderClassName.localeCompare("folderIcon") == 0) || (folderClassName.localeCompare("dirText") == 0) || (folderClassName.localeCompare("folderOptionContainer") == 0)) {
        folderName = folderButton.parentElement.textContent.trim();
    } else if (folderClassName.localeCompare("folderOptionEdit") == 0) {
        folderName = folderButton.parentElement.parentElement.textContent.trim();
    } else if (folderClassName.localeCompare("folderOptionDelete") == 0) {
        return;
    } else if (folderClassName.localeCompare("dirControl") == 0) {
        folderName = folderButton.textContent.trim();
    }

    if (folderName.localeCompare(selectedFolder) != 0) {
        selectedFolder = folderName;
        setFolderStyle(folderName);
        clearBookmarksView();
        bookmarksCore.selectFolder(folderName);
    }
}

function reloadCurrentFolder() {
    clearBookmarksView();
    bookmarksCore.selectFolder(selectedFolder);
}

function setFolderStyle(folderName) {
    let folderArray = document.getElementsByClassName("dirControl");
    let folderButtons = document.getElementsByClassName("folderOptionContainer");

    for (let i = 0; i < folderArray.length; i++) {
        let fName = folderArray.item(i).textContent.trim();
        if (folderName.localeCompare(fName) == 0) {
            activeFolder = folderArray.item(i);
            folderArray.item(i).style.cssText = "background-color: #f9f9fb; border-top: 1px solid #f9f9fb; border-bottom: 1px solid #f9f9fb; border-right: 1px solid #f9f9fb; border-left: 7px solid #56bcc5; padding-left: 4px;";
            folderArray.item(i).onmouseover = function() { folderArray.item(i).style.cssText = "background-color: #f9f9fb; border-top: 1px solid #f9f9fb; border-bottom: 1px solid #f9f9fb; border-right: 1px solid #f9f9fb; border-left: 7px solid #56bcc5; padding-left: 4px;"; }
            folderArray.item(i).onmouseleave = function() { folderArray.item(i).style.cssText = "background-color: #f9f9fb; border-top: 1px solid #f9f9fb; border-bottom: 1px solid #f9f9fb; border-right: 1px solid #f9f9fb; border-left: 7px solid #56bcc5; padding-left: 4px;"; }
            if (i > 0) {
                folderButtons.item(i - 1).style.display = "flex";
            }
        } else {
            folderArray.item(i).style.cssText = "background-color: transparent; border: 1px solid #ffffff; padding-left: 10px;";
            folderArray.item(i).onmouseover = function() { folderArray.item(i).style.cssText = "background-color: #f9f9fb; border: 1px solid #f9f9fb;"; }
            folderArray.item(i).onmouseleave = function() { folderArray.item(i).style.cssText = "background-color: transparent; border: 1px solid #ffffff;"; }
            if (i > 0) {
                folderButtons.item(i - 1).style.display = "none";
            }
        }
    }
}

function setFocusInStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../images/search.png";
}

function setFocusOutStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../../images/find.png";
}

function selectBookmark(clickEvent) {
    let clickedItem = clickEvent.target;
    let clickedItemClassName = clickedItem.className.trim();
    let bookmark = null;
    let editClicked = false;
    let deleteClicked = false;
    let saveClicked = false;
    let cancelClicked = false;

    if ((clickedItemClassName.localeCompare("itemTitleInput") == 0) || (clickedItemClassName.localeCompare("itemUrlInput") == 0)) {
        bookmark = clickedItem.parentElement.parentElement;
    } else if ((clickedItemClassName.localeCompare("itemButtonEdit") == 0) || (clickedItemClassName.localeCompare("itemButtonCancel") == 0) || (clickedItemClassName.localeCompare("itemButtonDelete") == 0) || (clickedItemClassName.localeCompare("itemButtonSave") == 0)) {
        bookmark = clickedItem.parentElement.parentElement;
        if (clickedItemClassName.localeCompare("itemButtonEdit") == 0) {
            editClicked = true;
        } else if (clickedItemClassName.localeCompare("itemButtonCancel") == 0) {
            cancelClicked = true;
        } else if (clickedItemClassName.localeCompare("itemButtonDelete") == 0) {
            deleteClicked = true;
        } else if (clickedItemClassName.localeCompare("itemButtonSave") == 0) {
            saveClicked = true;
        }
    } else if ((clickedItemClassName.localeCompare("itemTitle") == 0) || (clickedItemClassName.localeCompare("itemUrl") == 0) || (clickedItemClassName.localeCompare("itemIcon") == 0) || (clickedItemClassName.localeCompare("itemButtonContainerA") == 0) || (clickedItemClassName.localeCompare("itemButtonContainerB") == 0)) {
        bookmark = clickedItem.parentElement;
    } else {
        bookmark = clickedItem;
    }

    if (!isEditing) {
        selectedBookmark = bookmark;
        setBookmarkStyle(bookmark);

        if (onGroupSelection) {
            setControlButtonState(0, true);
            setControlButtonState(1, false);
            setControlButtonState(2, false);
            setControlButtonState(3, false);
            setControlButtonState(4, false);
            setControlButtonState(5, false);

            if ((cutedBookmark.length > 0) || (copiedBookmark.length > 0) || (groupSelectionCopied) || (groupSelectionCuted)) {
                setControlButtonState(6, false);
            } else { setControlButtonState(6, true); }

            let itemList = document.getElementsByClassName("item");
            if (itemList.length > 0) {
                setControlButtonState(9, false);
            } else { setControlButtonState(9, true); }
        } else if (onSearching) {
            setControlButtonState(0, true);
            setControlButtonState(1, false);
            setControlButtonState(2, false);
            setControlButtonState(3, false);
            setControlButtonState(4, true);
            setControlButtonState(5, true);
            setControlButtonState(6, true);
            setControlButtonState(9, true);
        } else {
            setControlButtonState(0, false);
            setControlButtonState(1, false);
            setControlButtonState(2, false);
            setControlButtonState(3, false);
            setControlButtonState(4, false);
            setControlButtonState(5, false);

            if ((cutedBookmark.length > 0) || (copiedBookmark.length > 0) || (groupSelectionCopied) || (groupSelectionCuted)) {
                setControlButtonState(6, false);
            } else { setControlButtonState(6, true); }

            let itemList = document.getElementsByClassName("item");
            if (itemList.length > 0) {
                setControlButtonState(9, false);
            } else { setControlButtonState(9, true); }
        }

        setControlButtonState(7, false);
        setControlButtonState(8, false);
    }
    if (editClicked) {
        editSelection();
    } else if (deleteClicked) {
        deleteBookmark();
    } else if (saveClicked) {
        saveEditing();
    } else if (cancelClicked) {
        cancel();
    }
}

function setBookmarkStyle(bookmark) {
    let bookmarkArray = document.getElementsByClassName("item");
    if (onGroupSelection) {
        if (bookmark.dataset.onSelection.localeCompare("false") == 0) {
            bookmark.style.cssText = "background-color: #f9f9fb; border: 2px solid #56bcc5;";
            bookmark.dataset.onSelection = "true";
            bookmark.childNodes.item(0).style.marginTop = "11px";
            bookmark.childNodes.item(0).style.marginLeft = "1.30%";
            bookmark.childNodes.item(1).style.marginLeft = "1.28%";
            bookmark.childNodes.item(2).style.marginLeft = "1.43%";
            bookmark.childNodes.item(3).style.marginTop = "5px";
            bookmark.childNodes.item(4).style.marginTop = "5px";
            bookmark.childNodes.item(3).style.paddingRight = "6px";
            bookmark.childNodes.item(4).style.paddingRight = "6px";

            selectedItems[itemSelected] = bookmark;
            itemSelected++;
            let notificationText = document.getElementById("selectionText");
            notificationText.innerText = "Bookmark(s) selected " + "(" + itemSelected + ")";
        } else if (itemSelected > 1) {
            bookmark.style.cssText = "background-color: #f9f9fb;";
            bookmark.dataset.onSelection = "false";
            bookmark.childNodes.item(0).style.marginTop = "12px";
            bookmark.childNodes.item(0).style.marginLeft = "1.35%";
            bookmark.childNodes.item(1).style.marginLeft = "1.35%";
            bookmark.childNodes.item(2).style.marginLeft = "1.35%";
            bookmark.childNodes.item(3).style.marginTop = "6px";
            bookmark.childNodes.item(4).style.marginTop = "6px";
            bookmark.childNodes.item(3).style.paddingRight = "7px";
            bookmark.childNodes.item(4).style.paddingRight = "7px";

            selectedItems.splice(selectedItems.indexOf(bookmark), 1);
            itemSelected--;
            let notificationText = document.getElementById("selectionText");
            notificationText.innerText = "Bookmark(s) selected " + "(" + itemSelected + ")";
        } else {
            selectedBookmark = bookmark;
            markBookmark();
        }

    } else {
        for (let i = 0; i < bookmarkArray.length; i++) {
            bookmarkArray.item(i).style.cssText = "background-color: #fdfdff;";
            bookmarkArray.item(i).childNodes.item(0).style.marginTop = "12px";
            bookmarkArray.item(i).childNodes.item(0).style.marginLeft = "1.35%";
            bookmarkArray.item(i).childNodes.item(1).style.marginLeft = "1.35%";
            bookmarkArray.item(i).childNodes.item(2).style.marginLeft = "1.35%";
            bookmarkArray.item(i).childNodes.item(3).style.marginTop = "6px";
            bookmarkArray.item(i).childNodes.item(4).style.marginTop = "6px";
            bookmarkArray.item(i).childNodes.item(3).style.paddingRight = "7px";
            bookmarkArray.item(i).childNodes.item(4).style.paddingRight = "7px";
        }
        bookmark.style.cssText = "background-color: #f9f9fb;";
    }
}

function clearBookmarkFocus(clickEvent) {
    if (isEditing || onGroupSelection) { return; }

    let section = clickEvent.target;
    if ((section.id.localeCompare("rightSection") == 0) || (section.id.localeCompare("itemView") == 0) || (section.id.localeCompare("itemList") == 0)) {
        selectedBookmark = null;
        let bookmarkArray = document.getElementsByClassName("item");
        for (let i = 0; i < bookmarkArray.length; i++) {
            bookmarkArray.item(i).style.cssText = "background-color: #fdfdff;";
        }

        setControlButtonState(0, false);
        setControlButtonState(1, true);
        setControlButtonState(2, true);
        setControlButtonState(3, true);
        setControlButtonState(4, true);
        setControlButtonState(5, true);

        if ((cutedBookmark.length > 0) || (copiedBookmark.length > 0) || (groupSelectionCopied) || (groupSelectionCuted)) {
            setControlButtonState(6, false);
        } else { setControlButtonState(6, true); }

        setControlButtonState(7, true);
        setControlButtonState(8, true);

        let itemList = document.getElementsByClassName("item");
        if (itemList.length > 0) {
            setControlButtonState(9, false);
        } else { setControlButtonState(9, true); }
    }
}

function contentScrolled() {
    let controlBar = document.getElementById("controlBar");
    let itemView = document.getElementById("itemView");

    if (itemView.scrollTop > 12) {
        controlBar.style.boxShadow = "0px 1px 2px 0px #d1d1d1";
    } else {
        controlBar.style.boxShadow = "none";
    }
}

function editSelection() {
    if (isEditing == false) {
        isEditing = true;
        selectedBookmark.childNodes.item(1).style.border = "2px solid #68e7f0";
        selectedBookmark.childNodes.item(1).style.background = "#fdfdfd";
        selectedBookmark.childNodes.item(1).childNodes.item(0).readOnly = false;
        selectedBookmark.childNodes.item(1).childNodes.item(0).style.cursor = "text";
        selectedBookmark.childNodes.item(2).style.border = "2px solid #68e7f0";
        selectedBookmark.childNodes.item(2).style.background = "#fdfdfd";
        selectedBookmark.childNodes.item(2).childNodes.item(0).readOnly = false;
        selectedBookmark.childNodes.item(2).childNodes.item(0).style.cursor = "text";
        selectedBookmark.childNodes.item(3).style.display = "none";
        selectedBookmark.childNodes.item(4).style.display = "flex";
        //fill field with titles value
        setControlButtonState(0, true);
        setControlButtonState(1, true);
        setControlButtonState(2, true);
        setControlButtonState(3, true);
        setControlButtonState(4, true);
        setControlButtonState(5, true);
        setControlButtonState(6, true);
        setControlButtonState(7, true);
        setControlButtonState(8, true);
        setControlButtonState(9, true);

        selectedBookmark.childNodes.item(1).childNodes.item(0).focus();
    }
}

function restoreDefaultState() {
    if (isEditing) {
        selectedBookmark.childNodes.item(1).style.border = "2px solid transparent";
        selectedBookmark.childNodes.item(1).style.background = "transparent";
        selectedBookmark.childNodes.item(1).childNodes.item(0).readOnly = true;
        selectedBookmark.childNodes.item(1).childNodes.item(0).style.cursor = "pointer";

        selectedBookmark.childNodes.item(2).style.border = "2px solid transparent";
        selectedBookmark.childNodes.item(2).style.background = "transparent";
        selectedBookmark.childNodes.item(2).childNodes.item(0).readOnly = true;
        selectedBookmark.childNodes.item(2).childNodes.item(0).style.cursor = "pointer";
        setBookmarkStyle(selectedBookmark);
    }

    if (onSearching) {
        setControlButtonState(0, true);
        setControlButtonState(1, false);
        setControlButtonState(2, false);
        setControlButtonState(3, false);
        setControlButtonState(4, true);
        setControlButtonState(5, true);
        setControlButtonState(6, true);
        setControlButtonState(9, true);
    } else {
        setControlButtonState(0, false);
        setControlButtonState(1, false);
        setControlButtonState(2, false);
        setControlButtonState(3, false);
        setControlButtonState(4, false);
        setControlButtonState(5, false);

        if ((cutedBookmark.length > 0) || (copiedBookmark.length > 0) || (groupSelectionCopied) || (groupSelectionCuted)) {
            setControlButtonState(6, false);
        } else { setControlButtonState(6, true); }

        let itemList = document.getElementsByClassName("item");
        if (itemList.length > 0) {
            setControlButtonState(9, false);
        } else { setControlButtonState(9, true); }
    }

    setControlButtonState(7, false);
    setControlButtonState(8, false);
}

function cancel() {
    if (isEditing == true) {
        restoreDefaultState();
        isEditing = false;

        selectedBookmark.childNodes.item(3).style.display = "flex";
        selectedBookmark.childNodes.item(4).style.display = "none";

        if (addingBookmark) {
            selectedBookmark.remove();
            addingBookmark = false;
            let itemList = document.getElementsByClassName("item");
            if (itemList.length < 1) {
                bookmarksViewEmpty(true);
            }
        } else {
            let bookmarkTitle = selectedBookmark.childNodes.item(1).childNodes.item(0).title;
            selectedBookmark.childNodes.item(1).childNodes.item(0).value = bookmarkTitle;

            let bookmarkUrl = selectedBookmark.childNodes.item(2).childNodes.item(0).title;
            selectedBookmark.childNodes.item(2).childNodes.item(0).value = bookmarkUrl;
        }
    }
}

function saveEditing() {
    if (isEditing == true) {
        restoreDefaultState();
        isEditing = false;

        selectedBookmark.childNodes.item(3).style.display = "flex";
        selectedBookmark.childNodes.item(4).style.display = "none";

        let bookmarkTitle = selectedBookmark.childNodes.item(1).childNodes.item(0).value;
        if (bookmarkTitle.length > 0) {
            selectedBookmark.childNodes.item(1).childNodes.item(0).title = bookmarkTitle;
        } else {
            selectedBookmark.childNodes.item(1).childNodes.item(0).value = "Ololan Browser";
            bookmarkTitle = "Ololan Browser";
        }

        let bookmarkUrl = selectedBookmark.childNodes.item(2).childNodes.item(0).value;
        if (bookmarkUrl.length > 0) {
            if (!bookmarkUrl.startsWith("http://") && !bookmarkUrl.startsWith("https://") &&
                !bookmarkUrl.startsWith("ftp://") && !bookmarkUrl.startsWith("ololan://")) {
                bookmarkUrl = "https://" + bookmarkUrl.trim();
                selectedBookmark.childNodes.item(2).childNodes.item(0).value = bookmarkUrl;
            }
            selectedBookmark.childNodes.item(2).childNodes.item(0).title = bookmarkUrl;
        } else {
            selectedBookmark.childNodes.item(2).childNodes.item(0).value = "https://www.ololan.com";
            bookmarkUrl = "https://www.ololan.com";
        }

        if (addingBookmark) {
            bookmarksCore.addBookmarkItem(bookmarkTitle, bookmarkUrl, selectedFolder);
            addingBookmark = false;
        } else {
            if (onSearching) {
                bookmarksCore.updateBookmarkItem(bookmarkTitle, bookmarkUrl, selectedBookmark.dataset.bookmarkFolder, selectedBookmark.dataset.bookmarkID);
            } else {
                bookmarksCore.updateBookmarkItem(bookmarkTitle, bookmarkUrl, selectedFolder, selectedBookmark.dataset.bookmarkID);
            }
        }
    }
}

function copyBookmark() {
    if (onGroupSelection) {
        groupSelectionCopied = true;
        selectedBookmark = null;
    } else if (selectedBookmark != null) {
        let bookmarkTitle = selectedBookmark.childNodes.item(1).childNodes.item(0).title;
        let bookmarkUrl = selectedBookmark.childNodes.item(2).childNodes.item(0).title;
        copiedBookmark = bookmarkTitle + " |+*ololanB*+| " + bookmarkUrl;
        copiedBookmarkIcon = selectedBookmark.childNodes.item(0).src;
    }
    cutedBookmark = "";
    sourceFolder = selectedFolder;
    restoreDefaultState();
}

function cutBookmark() {
    if (onGroupSelection) {
        groupSelectionCuted = true;
        selectedBookmark = null;
        for (let i = 0; i < selectedItems.length; i++) {
            selectedItems[i].remove();
        }
    } else if (selectedBookmark != null) {
        let bookmarkTitle = selectedBookmark.childNodes.item(1).childNodes.item(0).title;
        let bookmarkUrl = selectedBookmark.childNodes.item(2).childNodes.item(0).title;
        let bookmarkIndex = selectedBookmark.dataset.bookmarkID;
        copiedBookmarkIcon = selectedBookmark.childNodes.item(0).src;
        cutedBookmark = bookmarkTitle + "|+*ololanB*+|" + bookmarkUrl + "|+*ololanB*+|" + bookmarkIndex;
        selectedBookmark.innerHTML = "";
        selectedBookmark.style.height = "1px";
        selectedBookmark.remove();
    }

    copiedBookmark = "";
    sourceFolder = selectedFolder;
    setControlButtonState(0, false);
    setControlButtonState(1, true);
    setControlButtonState(2, true);
    setControlButtonState(3, true);
    setControlButtonState(4, true);
    setControlButtonState(5, true);

    if ((cutedBookmark.length > 0) || (copiedBookmark.length > 0) || (groupSelectionCopied) || (groupSelectionCuted)) {
        setControlButtonState(6, false);
    } else { setControlButtonState(6, true); }

    setControlButtonState(7, true);
    setControlButtonState(8, true);

    let itemList = document.getElementsByClassName("item");
    if (itemList.length > 0) {
        setControlButtonState(9, false);
    } else { setControlButtonState(9, true); }
}

function pasteCopiedBookmark() {
    if (selectedFolder.length > 0) {
        if (selectedItems.length > 0) {
            bookmarksViewEmpty(false);
            for (let i = 0; i < selectedItems.length; i++) {
                let bookmarkItem = createBookmark("paste");
                let bookmarkTitle = selectedItems[i].childNodes.item(1).childNodes.item(0).title;
                let bookmarkUrl = selectedItems[i].childNodes.item(2).childNodes.item(0).title;

                bookmarkItem.childNodes.item(0).src = selectedItems[i].childNodes.item(0).src;
                bookmarkItem.childNodes.item(1).childNodes.item(0).value = bookmarkTitle;
                bookmarkItem.childNodes.item(1).childNodes.item(0).title = bookmarkTitle;
                bookmarkItem.childNodes.item(2).childNodes.item(0).value = bookmarkUrl;
                bookmarkItem.childNodes.item(2).childNodes.item(0).title = bookmarkUrl;

                if (groupSelectionCopied) {
                    bookmarksCore.duplicateBookmarkItem(bookmarkTitle, bookmarkUrl.trim(), selectedFolder);
                } else if (groupSelectionCuted) {
                    bookmarksCore.moveBookmarkItem(bookmarkUrl.trim(), selectedItems[i].dataset.bookmarkID, sourceFolder, selectedFolder);
                    selectedItems.splice(i, 1);
                    i--;
                }
            }

            if (groupSelectionCuted) {
                groupSelectionCuted = false;
                onGroupSelection = false;
                document.getElementById("controlBar").style.height = "1px";
                let notification = document.getElementById("selectedNotification");
                notification.style.height = "1px";
                setTimeout(function() {
                    notification.style.height = "42px";
                    notification.classList.remove("notificationShowUp");
                }, 111);
            }

        } else if ((copiedBookmark.length > 2) && (!isEditing)) {
            bookmarksViewEmpty(false);
            let bookmarkItem = createBookmark("paste");
            let bookmarkData = copiedBookmark.split("|+*ololanB*+|");

            bookmarkItem.childNodes.item(0).src = copiedBookmarkIcon;
            bookmarkItem.childNodes.item(1).childNodes.item(0).value = bookmarkData[0];
            bookmarkItem.childNodes.item(1).childNodes.item(0).title = bookmarkData[0];
            bookmarkItem.childNodes.item(2).childNodes.item(0).value = bookmarkData[1];
            bookmarkItem.childNodes.item(2).childNodes.item(0).title = bookmarkData[1];
            bookmarksCore.duplicateBookmarkItem(bookmarkData[0], bookmarkData[1].trim(), selectedFolder);

        } else if ((cutedBookmark.length > 2) && (!isEditing)) {
            bookmarksViewEmpty(false);
            let bookmarkItem = createBookmark("paste");
            let bookmarkData = cutedBookmark.split("|+*ololanB*+|");

            bookmarkItem.childNodes.item(0).src = copiedBookmarkIcon;
            bookmarkItem.childNodes.item(1).childNodes.item(0).value = bookmarkData[0];
            bookmarkItem.childNodes.item(1).childNodes.item(0).title = bookmarkData[0];
            bookmarkItem.childNodes.item(2).childNodes.item(0).value = bookmarkData[1];
            bookmarkItem.childNodes.item(2).childNodes.item(0).title = bookmarkData[1];
            bookmarkItem.dataset.bookmarkID = bookmarkData[2];
            bookmarksCore.moveBookmarkItem(bookmarkData[1].trim(), bookmarkItem.dataset.bookmarkID, sourceFolder, selectedFolder);
            cutedBookmark = "";
            copiedBookmarkIcon = null;
        }
        setControlDefaultState();
    }
}

function createBookmark(createType) {
    let bookmarkItem = document.createElement("DIV");
    bookmarkItem.className = "item";

    let bookmarkItemIcon = document.createElement("IMG");
    bookmarkItemIcon.className = "itemIcon";
    bookmarkItemIcon.src = "../images/webpage.png";

    let bookmarkItemTitle = document.createElement("DIV");
    bookmarkItemTitle.className = "itemTitle";

    let bookmarkItemTitleInput = document.createElement("INPUT");
    bookmarkItemTitleInput.placeholder = "Type bookmark's title";
    bookmarkItemTitleInput.className = "itemTitleInput";
    bookmarkItemTitleInput.readOnly = true;

    let bookmarkItemUrl = document.createElement("DIV");
    bookmarkItemUrl.className = "itemUrl";

    let bookmarkItemUrlInput = document.createElement("INPUT");
    bookmarkItemUrlInput.placeholder = "Type bookmark's url";
    bookmarkItemUrlInput.className = "itemUrlInput";
    bookmarkItemUrlInput.readOnly = true;

    if (createType.localeCompare("add") == 0) {
        bookmarkItemTitleInput.title = "Ololan Browser";
        bookmarkItemUrlInput.title = "https://www.ololan.com";
    }

    let bookmarkContainerButtonA = document.createElement("DIV");
    bookmarkContainerButtonA.className = "itemButtonContainerA";

    let bookmarkEditButton = document.createElement("BUTTON");
    bookmarkEditButton.className = "itemButtonEdit";

    let bookmarkDeleteButton = document.createElement("BUTTON");
    bookmarkDeleteButton.className = "itemButtonDelete";

    let bookmarkContainerButtonB = document.createElement("DIV");
    bookmarkContainerButtonB.className = "itemButtonContainerB";

    let bookmarkSaveButton = document.createElement("BUTTON");
    bookmarkSaveButton.className = "itemButtonSave";

    let bookmarkCancelButton = document.createElement("BUTTON");
    bookmarkCancelButton.className = "itemButtonCancel";

    bookmarkItem.appendChild(bookmarkItemIcon);
    bookmarkItemTitle.appendChild(bookmarkItemTitleInput);
    bookmarkItem.appendChild(bookmarkItemTitle);
    bookmarkItemUrl.appendChild(bookmarkItemUrlInput);
    bookmarkItem.appendChild(bookmarkItemUrl);
    bookmarkContainerButtonA.appendChild(bookmarkEditButton);
    bookmarkContainerButtonA.appendChild(bookmarkDeleteButton);
    bookmarkItem.appendChild(bookmarkContainerButtonA);
    bookmarkContainerButtonB.appendChild(bookmarkSaveButton);
    bookmarkContainerButtonB.appendChild(bookmarkCancelButton);
    bookmarkItem.appendChild(bookmarkContainerButtonB);
    bookmarkItem.addEventListener("click", selectBookmark);
    bookmarkItem.addEventListener("dblclick", browseBookmark);

    let bookmarkList = document.getElementById("itemList");
    bookmarkList.appendChild(bookmarkItem);
    return bookmarkItem;
}

function deleteBookmark() {
    if (!isEditing) {
        forceRemoving();
        undoDeletion = false;
        let undoBox = document.getElementById("undoNotification");
        undoBox.style.display = "flex";
        setTimeout(function() { undoBox.classList.add("show"); }, 99);
        let itemList = document.getElementsByClassName("item");

        if ((selectedItems.length > 0) && (onGroupSelection)) {
            let notification = document.getElementById("selectedNotification");
            notification.style.display = "none";
            onGroupSelection = false;
            itemSelected = 0;

            for (let i = 0; i < selectedItems.length; i++) {
                if (!onSearching) {
                    selectedItems[i].dataset.bookmarkFolder = selectedFolder;
                }

                for (let j = 0; j < itemList.length; j++) {
                    if (selectedItems[i] == itemList.item(j)) {
                        selectedItems[i].dataset.listIndex = j;
                        break;
                    }
                }
                bookmarkListOnDeletion[bookmarkListOnDeletion.length] = selectedItems[i];
                bookmarksCore.setDeletedBookmark(selectedItems[i].childNodes.item(1).childNodes.item(0).title, selectedItems[i].childNodes.item(2).childNodes.item(0).title, selectedItems[i].dataset.bookmarkFolder, selectedItems[i].dataset.bookmarkID);
            }

            while (0 < selectedItems.length) {
                selectedItems[0].dataset.onSelection = "false";
                selectedItems[0].remove();
                selectedItems.splice(0, 1);
            }

            undoID = setTimeout(function() {
                deleteSelection();
                bookmarksCore.clearBookmarkDeleting();
                undoBox.style.display = "none";
                undoBox.classList.remove("show");
            }, 10000);

        } else if (selectedBookmark != null) {
            for (let i = 0; i < itemList.length; i++) {
                if (selectedBookmark == itemList.item(i)) {
                    selectedBookmark.dataset.listIndex = i;
                    break;
                }
            }

            if (!onSearching) {
                selectedBookmark.dataset.bookmarkFolder = selectedFolder;
            }

            selectedBookmark.classList.add("hide");
            bookmarkOnDeletion = selectedBookmark;
            selectedBookmark = null;
            bookmarksCore.setDeletedBookmark(bookmarkOnDeletion.childNodes.item(1).childNodes.item(0).title, bookmarkOnDeletion.childNodes.item(2).childNodes.item(0).title, bookmarkOnDeletion.dataset.bookmarkFolder, bookmarkOnDeletion.dataset.bookmarkID);

            setTimeout(function() { bookmarkOnDeletion.remove(); }, 130);
            undoID = setTimeout(function() {
                bookmarksCore.deleteBookmarkItem(bookmarkOnDeletion.childNodes.item(2).childNodes.item(0).title, bookmarkOnDeletion.dataset.bookmarkFolder, bookmarkOnDeletion.dataset.bookmarkID);
                bookmarksCore.clearBookmarkDeleting();
                undoID = -1;
                undoBox.style.display = "none";
                undoBox.classList.remove("show");
            }, 10000);
        }

        setControlButtonState(0, false);
        setControlButtonState(1, true);
        setControlButtonState(2, true);
        setControlButtonState(3, true);
        setControlButtonState(4, true);
        setControlButtonState(5, true);

        if ((cutedBookmark.length > 0) || (copiedBookmark.length > 0) || (groupSelectionCopied) || (groupSelectionCuted)) {
            setControlButtonState(6, false);
        } else { setControlButtonState(6, true); }

        setControlButtonState(7, true);
        setControlButtonState(8, true);

        setTimeout(function() {
            let itemList = document.getElementsByClassName("item");
            if (itemList.length > 1) {
                setControlButtonState(9, false);
            } else {
                setControlButtonState(9, true);
                bookmarksViewEmpty(true);
            }
        }, 113);
    }
}

function setupStart() {
    setFolderStyle("Bookmark bar");
    setControlDefaultState();
}

function deleteSelection() {
    undoID = -1;
    while (0 < bookmarkListOnDeletion.length) {
        bookmarksCore.deleteBookmarkItem(bookmarkListOnDeletion[0].childNodes.item(2).childNodes.item(0).title, bookmarkListOnDeletion[0].dataset.bookmarkFolder, bookmarkListOnDeletion[0].dataset.bookmarkID);
        bookmarkListOnDeletion.splice(0, 1);
    }
}

function clearSelectedFolder() {
    if ((document.getElementsByClassName("item").length > 0) && (!isEditing)) {
        let undoBox = document.getElementById("undoNotification");
        undoBox.style.display = "flex";
        setTimeout(function() { undoBox.classList.add("show"); }, 99);

        forceRemoving();
        onClearFolder = true;
        folderToClear = selectedFolder;
        selectedBookmark = null;
        let bookmarkItemList = document.getElementsByClassName("item");

        for (let i = 0; i < bookmarkItemList.length; i++) {
            bookmarksCore.setDeletedBookmark(bookmarkItemList.item(i).childNodes.item(1).childNodes.item(0).title, bookmarkItemList.item(i).childNodes.item(2).childNodes.item(0).title, selectedFolder, bookmarkItemList.item(i).dataset.bookmarkID);
        }

        clearBookmarksView();
        bookmarksViewEmpty(true)
        undoID = setTimeout(function() {
            undoBox.style.display = "none";
            undoBox.classList.remove("show");
            onClearFolder = false;
            folderToClear = "";
            undoID = -1;
            bookmarksCore.deleteBookmarkItem(selectedFolder);
            bookmarksCore.clearBookmarkDeleting();
        }, 10000);

        setControlButtonState(0, false);
        setControlButtonState(1, true);
        setControlButtonState(2, true);
        setControlButtonState(3, true);
        setControlButtonState(4, true);
        setControlButtonState(5, true);

        if ((cutedBookmark.length > 0) || (copiedBookmark.length > 0) || (groupSelectionCopied) || (groupSelectionCuted)) {
            setControlButtonState(6, false);
        } else { setControlButtonState(6, true); }

        setControlButtonState(7, true);
        setControlButtonState(8, true);

        let itemList = document.getElementsByClassName("item");
        if (itemList.length > 0) {
            setControlButtonState(9, false);
        } else { setControlButtonState(9, true); }
    }
}

function loadFolder(foldername, scrollDown) {
    let folderIcon = document.createElement("IMG");
    folderIcon.className = "folderIcon";
    folderIcon.src = "../images/bookmarkFolder.png";

    let folderText = document.createElement("DIV");
    folderText.className = "dirText";
    folderText.innerText = foldername;

    let folderControl = document.createElement("BUTTON");
    folderControl.className = "dirControl";
    folderControl.title = foldername;
    folderControl.appendChild(folderIcon);
    folderControl.appendChild(folderText);
    folderControl.addEventListener("click", selectFolder);

    if (foldername.localeCompare("Bookmark bar") != 0) {
        let folderOptionEdit = document.createElement("DIV");
        folderOptionEdit.className = "folderOptionEdit";
        folderOptionEdit.addEventListener("click", openDialogBoxTwo);

        let folderOptionDelete = document.createElement("DIV");
        folderOptionDelete.className = "folderOptionDelete";
        folderOptionDelete.addEventListener("click", deleteFolder);

        let folderOptionContainer = document.createElement("DIV");
        folderOptionContainer.className = "folderOptionContainer";
        folderOptionContainer.appendChild(folderOptionEdit);
        folderOptionContainer.appendChild(folderOptionDelete);
        folderControl.appendChild(folderOptionContainer);
    }

    let folderSection = document.getElementById("folderSection");
    folderSection.appendChild(folderControl);
    if (scrollDown) {
        folderSection.scrollBy(0, folderSection.clientHeight);
    }
}

function addFolder() {
    let fieldInput = document.getElementById("fieldInput");

    if (fieldInput.value.length > 0) {
        let folderList = document.getElementsByClassName("dirText");
        let folderArray = [];

        for (let i = 0; i < folderList.length; i++) {
            folderArray[folderArray.length] = folderList.item(i).textContent;
        }

        let folder = generateFolderName(fieldInput.value, folderArray);
        bookmarksCore.addFolder(folder);
        loadFolder(folder, true);
        closeDialogBox();
    }
}

function editFolder() {
    let editInput = document.getElementById("editInput");

    if (editInput.value.length > 0) {
        if (selectedFolder.trim().localeCompare(editInput.value.trim()) != 0) {
            let folderList = document.getElementsByClassName("dirText");
            let folderArray = [];
            let index = -1;

            for (let i = 0; i < folderList.length; i++) {
                if (selectedFolder.trim().localeCompare(folderList.item(i).textContent) != 0) {
                    folderArray[folderArray.length] = folderList.item(i).textContent;
                } else {
                    index = i;
                }
            }

            let folder = generateFolderName(editInput.value, folderArray);
            if (index > (-1)) {
                folderList.item(index).innerHTML = folder;
                bookmarksCore.changeFolderName(selectedFolder, folder);
                selectedFolder = folder;
            }
        }

        closeDialogBox();
    }
}

function deleteFolder() {
    let folderList = document.getElementsByClassName("dirText");
    for (let i = 1; i < folderList.length; i++) {
        if (selectedFolder.localeCompare(folderList.item(i).textContent) == 0) {
            forceRemoving();

            folderOnDeletion = folderList.item(i).parentElement;
            folderOnDeletion.dataset.folderID = i;
            folderList.item(i - 1).parentElement.click();

            undoDeletion = false;
            let undoBox = document.getElementById("undoNotification");
            undoBox.style.display = "flex";
            setTimeout(function() { undoBox.classList.add("show"); }, 99);

            undoID = setTimeout(function() {
                bookmarksCore.deleteFolder(folderOnDeletion.textContent);
                bookmarksCore.clearFolderDeleting();
                folderOnDeletion = null;
                undoBox.classList.remove("show");
                undoBox.style.display = "none";
            }, 10000);

            bookmarksCore.setDeletedFolder(folderList.item(i).textContent);
            folderList.item(i).parentElement.remove();
            break;
        }
    }
}

function removeFolder() {
    let folderList = document.getElementsByClassName("dirText");
    for (let i = 1; i < folderList.length;) {
        folderList.item(i).parentElement.remove();
    }

    selectedFolder = "Bookmark bar";
    setFolderStyle(selectedFolder);
}

function setLastAddedIndex(lastItemAdded) {
    if (selectedBookmark != null) {
        selectedBookmark.dataset.bookmarkID = lastItemAdded;
    }
}

function addBookmark() {
    if (!isEditing && !onGroupSelection) {
        bookmarksViewEmpty(false);
        addingBookmark = true;
        let bookmarkItem = createBookmark("add");
        let itemView = document.getElementById("itemView");
        itemView.scrollBy(0, itemView.clientHeight);
        bookmarkItem.dataset.onSelection = false;
        bookmarkItem.click();
        editSelection();
    }
}

function browseBookmark() {
    if (selectedBookmark != null) {
        window.open(selectedBookmark.childNodes.item(2).childNodes.item(0).title, "_blank");
    }
    //open in new tab via c++
}

function markBookmark() {
    if (onGroupSelection && (itemSelected == 1)) {
        selectedBookmark = selectedItems[0];
    }

    itemSelected = 0;
    while (selectedItems.length > 0) {
        selectedItems[0].dataset.onSelection = "false";
        selectedItems.splice(0, 1);
    }

    if (!onGroupSelection && (selectedBookmark != null) && !isEditing) {
        onGroupSelection = true;
        document.getElementById("controlBar").style.height = "52px";
        let notification = document.getElementById("selectedNotification");
        setTimeout(function() { notification.classList.add("notificationShowUp"); }, 111);

        let containerButtonA = document.getElementsByClassName("itemButtonContainerA");
        for (let i = 0; i < containerButtonA.length; i++) {
            containerButtonA.item(i).style.display = "none";
        }

        selectedBookmark.click();
    } else if (onGroupSelection == true) {
        onGroupSelection = false;
        let notification = document.getElementById("selectedNotification");
        notification.classList.add("notificationHide");
        document.getElementById("controlBar").style.height = "1px";

        setTimeout(function() {
            notification.classList.remove("notificationShowUp");
            notification.classList.remove("notificationHide");
        }, 113);

        let containerButtonA = document.getElementsByClassName("itemButtonContainerA");
        for (let i = 0; i < containerButtonA.length; i++) {
            containerButtonA.item(i).style.display = "flex";
        }

        selectedBookmark.click();
    }
}

function clearBookmarksView() {
    setControlDefaultState();
    let itemList = document.getElementById("itemList");
    itemList.innerHTML = "";
}

function bookmarksViewEmpty(state) {
    setControlDefaultState();
    let itemList = document.getElementById("itemList");
    let emptyView = document.getElementById("emptyView");
    let noResult = document.getElementById("noResult");
    noResult.style.display = "none";

    if (state) {
        itemList.style.display = "none";
        emptyView.style.display = "flex";
    } else {
        itemList.style.display = "block";
        emptyView.style.display = "none";
    }
}

function noBookmarkResult() {
    setControlDefaultState();
    let itemList = document.getElementById("itemList");
    let emptyView = document.getElementById("emptyView");
    let noResult = document.getElementById("noResult");
    itemList.style.display = "none";
    emptyView.style.display = "none";
    noResult.style.display = "flex";
}

function setControlButtonState(button, state) {
    let controlButton = document.getElementsByClassName("controlButton");
    let buttonIcon = document.getElementsByClassName("controlButtonIcon");

    controlButton.item(button).disabled = state;
    if (state) { buttonIcon.item(button).style.filter = "contrast(80%)"; } else {
        buttonIcon.item(button).style.filter = "contrast(100%)";
    }
}

function searchInBookmark() {
    let textBox = document.getElementById("searchInput");
    if (textBox.value.length > 0) {
        bookmarksCore.searchInBookmark(textBox.value);
        setControlDefaultState();
        onSearching = true;
        let folderArray = document.getElementsByClassName("dirControl");
        let folderButtons = document.getElementsByClassName("folderOptionContainer");

        for (let i = 0; i < folderArray.length; i++) {
            let fName = folderArray.item(i).textContent.trim();
            if (selectedFolder.localeCompare(fName) == 0) {
                folderArray.item(i).style.cssText = "background-color: transparent; border: 1px solid #f2f5f5; padding-left: 10px;";
                folderArray.item(i).onmouseover = function() { folderArray.item(i).style.cssText = "background-color: #e2e5e5; border: 1px solid #e2e5e5;"; }
                folderArray.item(i).onmouseleave = function() { folderArray.item(i).style.cssText = "background-color: transparent; border: 1px solid #f2f5f5;"; }
                if (i > 0) {
                    folderButtons.item(i - 1).style.display = "none";
                }
                break;
            }
        }
        selectedFolder = "";
    } else if (activeFolder != null) {
        activeFolder.click();
    }
}

function closeDialogBox() {
    let dialogBox = document.getElementsByClassName("dialogContainer");
    dialogBox.item(0).style.display = "none";
}

function openDialogBoxOne() {
    let fieldInput = document.getElementById("fieldInput");
    fieldInput.value = "";

    let dialogBoxOne = document.getElementById("dialogBoxOne");
    dialogBoxOne.style.display = "block";

    let dialogBoxTwo = document.getElementById("dialogBoxTwo");
    dialogBoxTwo.style.display = "none";

    let dialogBox = document.getElementsByClassName("dialogContainer");
    dialogBox.item(0).style.display = "flex";

    fieldInput.focus();
}

function openDialogBoxTwo() {
    let editInput = document.getElementById("editInput");
    editInput.value = selectedFolder;

    let dialogBoxOne = document.getElementById("dialogBoxOne");
    dialogBoxOne.style.display = "none";

    let dialogBoxTwo = document.getElementById("dialogBoxTwo");
    dialogBoxTwo.style.display = "block";

    let dialogBox = document.getElementsByClassName("dialogContainer");
    dialogBox.item(0).style.display = "flex";

    editInput.focus();
}

function setControlDefaultState() {
    setControlButtonState(0, false);
    setControlButtonState(1, true);
    setControlButtonState(2, true);
    setControlButtonState(3, true);
    setControlButtonState(4, true);
    setControlButtonState(5, true);

    if ((cutedBookmark.length > 0) || (copiedBookmark.length > 0) || (groupSelectionCopied) || (groupSelectionCuted)) {
        setControlButtonState(6, false);
    } else { setControlButtonState(6, true); }

    setControlButtonState(7, true);
    setControlButtonState(8, true);

    let itemList = document.getElementsByClassName("item");
    if (itemList.length > 0) {
        setControlButtonState(9, false);
    } else { setControlButtonState(9, true); }
}

function generateFolderName(foldername, folderArray) {
    let name = foldername.trim();
    let result = "";
    let occurence = 0;
    let storedOccurence = 0;
    let searchCount = 1;
    let count = 0;

    for (let i = 0; i < folderArray.length; i++) {
        if (folderArray[i].trim().localeCompare(name) == 0) {
            occurence++;
        }

        if ((occurence > 0) && (i == (folderArray.length - 1))) {
            storedOccurence = storedOccurence + occurence;
            occurence = 0;

            if (searchCount == 1) {
                name = name + " (" + storedOccurence + ")";
                result = name;
            } else if (searchCount == (count + 1)) {
                name = foldername + " (" + storedOccurence + ")";
                result = name;
            }

            i = -1;
            count = searchCount;
            searchCount++;
        }
    }

    return (result.length > 0 ? result : name);
}

function setControlBarVisible() {
    console.log(document.body.clientHeight);
    let controlBar = document.getElementById("middleSection");
    if (document.body.clientHeight <= 430) {
        controlBar.style.display = "none";
    } else {
        controlBar.style.display = "flex";
    }
}

function undoRemoving() {
    undoDeletion = true;
    let undoBox = document.getElementById("undoNotification");
    undoBox.style.display = "none";
    undoBox.classList.remove("show");
    clearTimeout(undoID);
    undoID = -1;

    if (folderOnDeletion != null) {
        let folderList = document.getElementsByClassName("dirControl");

        if (folderList.length > parseInt(folderOnDeletion.dataset.folderID)) {
            folderList.item(parseInt(folderOnDeletion.dataset.folderID) - 0).insertAdjacentElement("beforebegin", folderOnDeletion);
        } else {
            folderList.item(parseInt(folderOnDeletion.dataset.folderID) - 1).insertAdjacentElement("afterend", folderOnDeletion);
        }

        folderOnDeletion.childNodes.item(2).style.display = "none";
        folderOnDeletion = null;
        bookmarksCore.clearFolderDeleting();
    } else if (bookmarkOnDeletion != null) {
        if (bookmarkOnDeletion.dataset.bookmarkFolder.localeCompare(selectedFolder) == 0) {
            let itemList = document.getElementsByClassName("item");
            let emptySpace = document.getElementById("itemView");
            bookmarkOnDeletion.classList.remove("hide");
            bookmarksViewEmpty(false);

            if (itemList.length < 1) {
                let bookmarkList = document.getElementById("itemList");
                bookmarkList.appendChild(bookmarkOnDeletion);
            } else if (itemList.length > parseInt(bookmarkOnDeletion.dataset.listIndex)) {
                itemList.item(parseInt(bookmarkOnDeletion.dataset.listIndex)).insertAdjacentElement("beforebegin", bookmarkOnDeletion);
            } else {
                itemList.item(parseInt(bookmarkOnDeletion.dataset.listIndex) - 1).insertAdjacentElement("afterend", bookmarkOnDeletion);
            }

            emptySpace.click();
        }

        bookmarksCore.clearBookmarkDeleting();
        bookmarkOnDeletion = null;
    } else if (bookmarkListOnDeletion.length > 0) {
        if (selectedFolder.localeCompare(bookmarkListOnDeletion[0].dataset.bookmarkFolder) == 0) {
            let itemList = document.getElementsByClassName("item");
            let emptySpace = document.getElementById("itemView");
            bookmarksViewEmpty(false);

            while (bookmarkListOnDeletion.length > 0) {
                bookmarkListOnDeletion[0].classList.remove("hide");

                if (itemList.length < 1) {
                    let bookmarkList = document.getElementById("itemList");
                    bookmarkList.appendChild(bookmarkListOnDeletion[0]);
                } else if (itemList.length > parseInt(bookmarkListOnDeletion[0].dataset.listIndex)) {
                    itemList.item(parseInt(bookmarkListOnDeletion[0].dataset.listIndex)).insertAdjacentElement("beforebegin", bookmarkListOnDeletion[0]);
                } else {
                    itemList.item(itemList.length - 1).insertAdjacentElement("afterend", bookmarkListOnDeletion[0]);
                }
                bookmarkListOnDeletion.splice(0, 1);
            }

            emptySpace.click();
        }
        bookmarksCore.clearBookmarkDeleting();
    } else if (onClearFolder) {
        bookmarksCore.clearBookmarkDeleting();

        if (folderToClear.localeCompare(selectedFolder) == 0) {
            clearBookmarksView();
            bookmarksCore.selectFolder(folderToClear);
        }
        onClearFolder = false;
        folderToClear = "";
    }
}

function forceRemoving() {
    if (undoID != (-1)) {
        clearTimeout(undoID);
        undoID = -1;
    }

    if (folderOnDeletion != null) {
        bookmarksCore.deleteFolder(folderOnDeletion.childNodes.item(1).textContent);
        bookmarksCore.clearFolderDeleting();
        folderOnDeletion.remove();
        folderOnDeletion = null;
    } else if (bookmarkOnDeletion != null) {
        bookmarksCore.deleteBookmarkItem(bookmarkOnDeletion.childNodes.item(2).childNodes.item(0).title, bookmarkOnDeletion.dataset.bookmarkFolder, bookmarkOnDeletion.dataset.bookmarkID);
        bookmarksCore.clearBookmarkDeleting();
    } else if (bookmarkListOnDeletion.length > 0) {
        deleteSelection();
        bookmarksCore.clearBookmarkDeleting();
    } else if (onClearFolder) {
        bookmarksCore.deleteBookmarkItem(folderToClear);
        bookmarksCore.clearBookmarkDeleting();
    }
}

function openInNewTab() {
    bookmarksCore.openInNewTab(selectedBookmark.childNodes.item(2).childNodes.item(0).title);
}

function openInNewWindow() {
    bookmarksCore.openInNewWindow(selectedBookmark.childNodes.item(2).childNodes.item(0).title);
}

function openInPrivateWindow() {
    bookmarksCore.openInPrivateWindow(selectedBookmark.childNodes.item(2).childNodes.item(0).title);
}

function setEditingItem(index) {
    let itemList = document.getElementsByClassName("item");
    for (let i = 0; i < itemList.length; i++) {
        if (index.localeCompare(itemList.item(i).dataset.bookmarkID) == 0) {
            let itemView = document.getElementById("itemView");
            itemView.scrollTo(itemList.item(i).getBoundingClientRect().x, itemList.item(i).getBoundingClientRect().y);
            itemList.item(i).click();
            break;
        }
    }
    setTimeout(editSelection(), 2000);
}

function deleteBookmarkBar(index) {
    if (!onGroupSelection && !isEditing) {
        let itemList = document.getElementsByClassName("item");
        for (let i = 0; i < itemList.length; i++) {
            if (index.localeCompare(itemList.item(i).dataset.bookmarkID) == 0) {
                let itemView = document.getElementById("itemView");
                itemView.scrollTo(itemList.item(i).getBoundingClientRect().x, itemList.item(i).getBoundingClientRect().y);
                itemList.item(i).click();
                deleteBookmark();
                break;
            }
        }
    }
}