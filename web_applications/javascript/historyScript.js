"use strict";

let runDefault = true;
let inPCHistory = true;
let groupDeletingButton = null;
let historyCore = null;

function setupConnector() {
    new QWebChannel(qt.webChannelTransport, function(ololanChannel) {
        historyCore = ololanChannel.objects.historyManager;
    });
}

function loadVisitedDate(day, month, year, type) {
    let dayDate = document.createElement("SPAN");
    dayDate.className = "dayDate";
    dayDate.innerHTML = day + ", " + month + ", " + year;

    let dialogButton = document.createElement("BUTTON");
    dialogButton.className = "deleteButton";
    dialogButton.title = "Delete this date from history";
    dialogButton.type = "button";
    dialogButton.addEventListener("click", openDialogBoxOne);

    let dateContainer = document.createElement("DIV");
    dateContainer.className = "visitedDate";
    dateContainer.appendChild(dayDate);
    dateContainer.appendChild(dialogButton);

    if (type.localeCompare("add") == 0) {
        let container = document.getElementById("historyItemContainer");
        container.appendChild(dateContainer);
    } else if (type.localeCompare("insert") == 0) {
        let dateArray = document.getElementsByClassName("visitedDate");
        dateArray.item(0).insertAdjacentElement("beforebegin", dateContainer);
    }
}

function loadHistoryItem(icon, host, url, title, time, index) {
    let historyItemTime = document.createElement("SPAN");
    historyItemTime.className = "visitedTime";
    historyItemTime.innerHTML = time;

    let historyItemUrl = document.createElement("SPAN");
    historyItemUrl.className = "itemUrl";
    historyItemUrl.innerHTML = host;

    let historyItemIcon = document.createElement("IMG");
    historyItemIcon.className = "itemIcon";
    historyItemIcon.src = icon;

    let historyItemTitle = document.createElement("SPAN");
    historyItemTitle.className = "itemTitle";
    historyItemTitle.innerHTML = title;

    let removeButton = document.createElement("BUTTON");
    removeButton.className = "closeButton";
    removeButton.title = "Delete from History";
    removeButton.type = "button";
    removeButton.addEventListener("click", removeItem);

    let historyItemLink = document.createElement("A");
    historyItemLink.className = "itemLink";
    historyItemLink.title = url;
    historyItemLink.href = url;
    historyItemLink.target = "_blank";
    historyItemLink.appendChild(historyItemIcon);
    historyItemLink.appendChild(historyItemTitle);
    historyItemLink.appendChild(document.createElement("DIV"));
    historyItemLink.appendChild(historyItemUrl);
    historyItemLink.appendChild(document.createElement("DIV"));
    historyItemLink.appendChild(historyItemTime);
    historyItemLink.appendChild(document.createElement("DIV"));
    historyItemLink.appendChild(removeButton);
    historyItemLink.addEventListener("click", processItemClick);

    let historyItem = document.createElement("DIV");
    historyItem.className = "item";
    historyItem.dataset.historyID = index;
    historyItem.appendChild(historyItemLink);

    let container = document.getElementById("historyItemContainer");
    container.appendChild(historyItem);
}

function insertHistoryItem(icon, host, url, title, time, index) {
    let historyItemTime = document.createElement("SPAN");
    historyItemTime.className = "visitedTime";
    historyItemTime.innerHTML = time;

    let historyItemUrl = document.createElement("SPAN");
    historyItemUrl.className = "itemUrl";
    historyItemUrl.innerHTML = host;

    let historyItemIcon = document.createElement("IMG");
    historyItemIcon.className = "itemIcon";
    historyItemIcon.src = icon;

    let historyItemTitle = document.createElement("SPAN");
    historyItemTitle.className = "itemTitle";
    historyItemTitle.innerHTML = title;

    let removeButton = document.createElement("BUTTON");
    removeButton.className = "closeButton";
    removeButton.title = "Delete from History";
    removeButton.type = "button";
    removeButton.addEventListener("click", removeItem);

    let historyItemLink = document.createElement("A");
    historyItemLink.className = "itemLink";
    historyItemLink.title = url;
    historyItemLink.href = url;
    historyItemLink.target = "_blank";
    historyItemLink.appendChild(historyItemIcon);
    historyItemLink.appendChild(historyItemTitle);
    historyItemLink.appendChild(document.createElement("DIV"));
    historyItemLink.appendChild(historyItemUrl);
    historyItemLink.appendChild(document.createElement("DIV"));
    historyItemLink.appendChild(historyItemTime);
    historyItemLink.appendChild(document.createElement("DIV"));
    historyItemLink.appendChild(removeButton);
    historyItemLink.addEventListener("click", processItemClick);

    let historyItem = document.createElement("DIV");
    historyItem.className = "item";
    historyItem.dataset.historyID = index;
    historyItem.appendChild(historyItemLink);

    let historyDateArray = document.getElementsByClassName("visitedDate");
    historyDateArray.item(0).insertAdjacentElement("afterend", historyItem);
}

function historyViewEmpty(condition) {
    let emptyViewArray = document.getElementsByClassName("emptyView");
    let eViewMessageArray = document.getElementsByClassName("eViewMessage");
    let thisPCHistory = document.getElementById("historyItemContainer");
    let noResultArray = document.getElementsByClassName("noResult");
    noResultArray[0].style.display = "none";

    if (condition == true) {
        thisPCHistory.style.display = "flex";
        emptyViewArray[0].style.display = "flex";
        eViewMessageArray[0].style.width = "290px";
    } else {
        thisPCHistory.style.display = "block";
        emptyViewArray[0].style.display = "none";
    }
}

function clearAllHistoryView() {
    let visitedDateArray = document.getElementsByClassName("visitedDate");
    let itemArray = document.getElementsByClassName("item");

    while (visitedDateArray.length > 0) {
        visitedDateArray[0].remove();
    }

    while (itemArray.length > 0) {
        itemArray[0].remove();
    }
}

function contentScroll() {
    let bar = document.getElementById("controlBar");
    let pageElement = document.getElementById("itemList");

    if (pageElement.scrollTop > 12) {
        bar.style.boxShadow = "0px 1px 2px 0px #d1d1d1";
    } else {
        bar.style.boxShadow = "none";
    }
}

function clearBrowsingData() {
    closeDialogBoxTwo();
    clearAllHistoryView();
    historyCore.clearBrowsingData();
}

function loadThisPCHistory() {
    let textBox = document.getElementById("searchInput");
    if (textBox.value.length > 0) { searchInHistory(); } else { historyCore.loadHistoryItems(); }
}

function searchInHistory() {
    let textBox = document.getElementById("searchInput");
    historyCore.searchInHistory(textBox.value);
}

function noHistoryResult() {
    let noResultArray = document.getElementsByClassName("noResult");
    noResultArray[0].style.display = "flex";
}

function processItemClick(clickEvent) {
    if (!runDefault) {
        clickEvent.preventDefault();
        runDefault = true;
    }
}

function removeItem(clickEvent) {
    runDefault = false;
    let itemContainer = clickEvent.target.parentElement.parentElement;
    let isGroupRemoving = false;

    if (itemContainer.previousElementSibling != null) {
        if (itemContainer.previousElementSibling.className.localeCompare("visitedDate") == 0) {
            if (itemContainer.nextElementSibling != null) {
                if (itemContainer.nextElementSibling.className.localeCompare("visitedDate") == 0) { isGroupRemoving = true; }
            } else { isGroupRemoving = true; }
        }
    }

    if (isGroupRemoving) {
        groupDeletingButton = itemContainer.previousElementSibling.getElementsByClassName("deleteButton")[0];
        removeGroupItem();
    } else {
        itemContainer.classList.add("hide");
        setTimeout(function() { itemContainer.remove(); }, 130);
    }

    historyCore.removeHistoryItem(itemContainer.dataset.historyID);
}

function removeGroupItem() {
    if (!groupDeletingButton) { return; }

    let browsingDate = groupDeletingButton.parentElement;
    let nextBrowsingDate;
    let browsingDateArray = document.getElementsByClassName("visitedDate");

    for (let i = 0; i < browsingDateArray.length; i++) {
        if ((browsingDateArray[i] === browsingDate) && (i != (browsingDateArray.length - 1))) {
            nextBrowsingDate = browsingDateArray[i + 1];
            break;
        }
    }

    if (nextBrowsingDate != null) {
        let pageToDelete = document.getElementsByClassName("item");
        let rect1 = browsingDate.getBoundingClientRect();
        let rect2 = nextBrowsingDate.getBoundingClientRect();

        for (let i = 0; i < pageToDelete.length; i++) {
            let rect3 = pageToDelete[i].getBoundingClientRect();
            if ((rect1.bottom < rect3.top) && (rect2.top > rect3.bottom)) {
                historyCore.removeHistoryItem(pageToDelete.item(i).dataset.historyID);
                pageToDelete.item(i).remove();
                i--;
            }
        }

        browsingDate.classList.add("hide");
        setTimeout(function() { browsingDate.remove(); }, 130);
    } else {
        let pageToDelete = document.getElementsByClassName("item");
        let rect1 = browsingDate.getBoundingClientRect();

        for (let i = 0; i < pageToDelete.length; i++) {
            let rect3 = pageToDelete[i].getBoundingClientRect();
            if (rect1.bottom < rect3.top) {
                historyCore.removeHistoryItem(pageToDelete.item(i).dataset.historyID);
                pageToDelete.item(i).remove();
                i--;
            }
        }

        browsingDate.classList.add("hide");
        setTimeout(function() { browsingDate.remove(); }, 130);
    }

    closeDialogBoxOne();
}

function openDialogBoxOne(clickEvent) {
    if (groupDeletingButton == null) {
        groupDeletingButton = clickEvent.target;
        let dialogContainer = document.getElementsByClassName("dialogContainer");
        document.getElementById("dialogBoxOne").style.display = "block";
        document.getElementById("dialogBoxTwo").style.display = "none";
        dialogContainer[0].style.display = "flex";
    }
}

function openDialogBoxTwo() {
    let dialogContainer = document.getElementsByClassName("dialogContainer");
    document.getElementById("dialogBoxOne").style.display = "none";
    document.getElementById("dialogBoxTwo").style.display = "block";
    dialogContainer[0].style.display = "flex";
}

function closeDialogBoxOne() {
    if (groupDeletingButton) {
        groupDeletingButton = null;
        let dialogContainer = document.getElementsByClassName("dialogContainer");
        dialogContainer[0].style.display = "none";
    }
}

function closeDialogBoxTwo() {
    groupDeletingButton = null;
    let dialogContainer = document.getElementsByClassName("dialogContainer");
    dialogContainer[0].style.display = "none";
}

function setFocusInStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../images/search.png";
}

function setFocusOutStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../../images/find.png";
}

function setWidgetSize() {
    let htmlBody = document.body;
    let pageBody = document.getElementById("pageBody");
    pageBody.style.height = (htmlBody.getBoundingClientRect().height - 69) + "px";
    pageBody.style.maxHeight = "87%";
}

function setupStart() {
    setWidgetSize();
    document.body.onresize = function() { setWidgetSize() };
}