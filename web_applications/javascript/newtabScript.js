"use strict";
let pageCore = null;

function setupConnector() {
    new QWebChannel(qt.webChannelTransport, function(ololanChannel) {
        pageCore = ololanChannel.objects.startPageManager;
    });
}

function setFocusInStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../images/search.png";
}

function setFocusOutStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../../images/find.png";
}

function processUri(event) {
    if (event.keyCode === 13) {
        let input = document.getElementById("searchInput");
        document.body.focus();
        pageCore.processQuery(input.value);
        input.value = "";
    }
}

function setCurrentPlaceholder(placeholderText) {
    document.getElementById("searchInput").placeholder = ("Search with " + placeholderText);
}

function createItem(webTitle, url, icon, withCell) {
    let itemIcon = document.createElement("IMG");
    itemIcon.className = "hItemLogo";
    itemIcon.src = icon;

    let hItemTitle = document.createElement("DIV");
    hItemTitle.className = "hItemTitle";
    hItemTitle.textContent = webTitle;

    let hItemContentA = document.createElement("DIV");
    hItemContentA.className = "hItemContent";
    hItemContentA.appendChild(itemIcon);

    let hItemContentB = document.createElement("DIV");
    hItemContentB.className = "hItemContent";
    hItemContentB.appendChild(hItemTitle);

    let itemLink = document.createElement("A");
    itemLink.className = "itemLink";
    itemLink.href = url;
    itemLink.appendChild(hItemContentA);
    itemLink.appendChild(hItemContentB);

    let hItem = document.createElement("DIV");
    hItem.className = "hItem";
    hItem.appendChild(itemLink);
    hItem.title = webTitle;

    if (withCell) {
        let itemCell = document.createElement("DIV");
        itemCell.className = "itemCell";
        itemCell.appendChild(hItem);
        return itemCell;
    } else {
        return hItem;
    }
}

function addItem(title, url, icon) {
    let hItemList = document.getElementsByClassName("hItem");
    if ((hItemList.length == 0) || (hItemList.length == 1) || (hItemList.length == 2) ||
        (hItemList.length == 3) || (hItemList.length == 4)) {
        document.getElementById("historyArea").appendChild(createItem(title, url, icon, true));
    } else if (hItemList.length < 6) {
        document.getElementsByClassName("itemCell").item(0).appendChild(createItem(title, url, icon, false));
    } else if (hItemList.length < 7) {
        document.getElementsByClassName("itemCell").item(1).appendChild(createItem(title, url, icon, false));
    } else if (hItemList.length < 8) {
        document.getElementsByClassName("itemCell").item(2).appendChild(createItem(title, url, icon, false));
    } else if (hItemList.length < 9) {
        document.getElementsByClassName("itemCell").item(3).appendChild(createItem(title, url, icon, false));
    } else if (hItemList.length < 10) {
        document.getElementsByClassName("itemCell").item(4).appendChild(createItem(title, url, icon, false));
    }
}