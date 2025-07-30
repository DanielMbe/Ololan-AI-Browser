"use strict";
let downloadsCore = null;
let buttonTargetParent = null;

function setupConnector() {
    new QWebChannel(qt.webChannelTransport, function(ololanChannel) {
        downloadsCore = ololanChannel.objects.downloadsManager;
    });
}

function openDialogBoxOne() {
    let dialogContainer = document.getElementsByClassName("dialogContainer");
    dialogContainer[0].style.display = "flex";
    let dialogBoxTwo = document.getElementById("dialogBoxTwo");
    dialogBoxTwo.style.display = "none";
    let dialogBoxOne = document.getElementById("dialogBoxOne");
    dialogBoxOne.style.display = "block";
}

function closeDialogBoxOne() {
    let dialogContainer = document.getElementsByClassName("dialogContainer");
    dialogContainer[0].style.display = "none";
    let dialogBoxOne = document.getElementById("dialogBoxOne");
    dialogBoxOne.style.display = "none"
}

function openDialogBoxTwo(clickEvent) {
    buttonTargetParent = clickEvent.target.parentElement;
    let dialogContainer = document.getElementsByClassName("dialogContainer");
    dialogContainer[0].style.display = "flex";
    let dialogBoxOne = document.getElementById("dialogBoxOne");
    dialogBoxOne.style.display = "none";
    let dialogBoxTwo = document.getElementById("dialogBoxTwo");
    dialogBoxTwo.style.display = "block";
}

function closeDialogBoxTwo() {
    let dialogContainer = document.getElementsByClassName("dialogContainer");
    dialogContainer[0].style.display = "none";
    let dialogBoxTwo = document.getElementById("dialogBoxTwo");
    dialogBoxTwo.style.display = "none";
}

function setFocusInStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../images/search.png";
}

function setFocusOutStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../../images/find.png";
}

function searchInDownloads() {
    let textBox = document.getElementById("searchInput");
    downloadsCore.searchInDownloads(textBox.value);
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

function loadDownloadedDate(day, month, year, index, type) {
    let dayDate = document.createElement("SPAN");
    dayDate.className = "dayDate";
    dayDate.innerHTML = day + ", " + month + ", " + year;

    let dialogButton = document.createElement("BUTTON");
    dialogButton.className = "deleteButton";
    dialogButton.title = "Delete downloads of this date";
    dialogButton.type = "button";
    dialogButton.addEventListener("click", openDialogBoxTwo);

    let dateContainer = document.createElement("DIV");
    dateContainer.dataset.dateIndex = index;
    dateContainer.className = "downloadedDate";
    dateContainer.appendChild(dayDate);
    dateContainer.appendChild(dialogButton);

    if (type.localeCompare("add") == 0) {
        let container = document.getElementById("itemList");
        container.appendChild(dateContainer);
    } else if (type.localeCompare("insert") == 0) {
        let dateArray = document.getElementsByClassName("downloadedDate");
        dateArray.item(0).insertAdjacentElement("beforebegin", dateContainer);
    }
}

function loadDownloadItem(index, title, url, size, downloadSize, icon, state, isMoved) {
    let item = createDownloadItem(index, title, url, size, downloadSize, icon, state, isMoved);
    let itemList = document.getElementById("itemList");
    itemList.appendChild(item);
}

function createDownloadItem(index, title, url, size, downloadSize, icon, state, isMoved) {
    let itemIcon = document.createElement("IMG");
    itemIcon.className = "itemIcon";
    itemIcon.src = icon;

    let iconSection = document.createElement("DIV");
    iconSection.className = "iconSection";
    iconSection.appendChild(itemIcon);

    let itemTitle = document.createElement("DIV");
    itemTitle.className = "itemTitle";
    itemTitle.textContent = title;

    if (isMoved.localeCompare("false") == 0) {
        itemTitle.style.textDecoration = "line-through";
    }

    let urlLink = document.createElement("A");
    urlLink.className = "urlLink";
    urlLink.textContent = url;
    urlLink.href = url;

    let itemUrl = document.createElement("DIV");
    itemUrl.className = "itemUrl";
    itemUrl.appendChild(urlLink);

    let itemSize = document.createElement("DIV");
    itemSize.className = "itemSize";

    let itemTransferRate = document.createElement("DIV");
    itemTransferRate.className = "itemTransferRate";
    itemTransferRate.textContent = state;

    let itemTransferData = document.createElement("DIV");
    itemTransferData.className = "itemTransfertData";
    itemTransferData.appendChild(itemSize);
    itemTransferData.appendChild(itemTransferRate);

    let pauseButton = document.createElement("BUTTON");
    pauseButton.className = "pauseButton";
    pauseButton.title = "Pause downloading";
    pauseButton.addEventListener("click", pauseDownload);

    let stopButtonA = document.createElement("BUTTON");
    stopButtonA.className = "stopButton";
    stopButtonA.title = "Stop downloading";
    stopButtonA.addEventListener("click", stopDownload);

    let itemButtonContainerA = document.createElement("DIV");
    itemButtonContainerA.className = "itemButtonContainerA";
    itemButtonContainerA.appendChild(pauseButton);
    itemButtonContainerA.appendChild(stopButtonA);

    let openFolderButton = document.createElement("BUTTON");
    openFolderButton.className = "openFolderButton";
    openFolderButton.title = "Open file location";
    openFolderButton.addEventListener("click", openDownloadLocation);

    let deleteItemButtonA = document.createElement("BUTTON");
    deleteItemButtonA.className = "deleteItemButton";
    deleteItemButtonA.title = "Delete file from list";
    deleteItemButtonA.addEventListener("click", deleteDownloadItem);

    let itemButtonContainerB = document.createElement("DIV");
    itemButtonContainerB.className = "itemButtonContainerB";
    itemButtonContainerB.appendChild(openFolderButton);
    itemButtonContainerB.appendChild(deleteItemButtonA);

    let resumeButton = document.createElement("BUTTON");
    resumeButton.className = "resumeButton";
    resumeButton.title = "Resume download";
    resumeButton.addEventListener("click", resumeDownload);

    let stopButtonB = document.createElement("BUTTON");
    stopButtonB.className = "stopButton";
    stopButtonB.title = "Stop downloading";
    stopButtonB.addEventListener("click", stopDownload);

    let itemButtonContainerC = document.createElement("DIV");
    itemButtonContainerC.className = "itemButtonContainerC";
    itemButtonContainerC.appendChild(resumeButton);
    itemButtonContainerC.appendChild(stopButtonB);

    let redownloadButton = document.createElement("BUTTON");
    redownloadButton.className = "redownloadButton";
    redownloadButton.title = "Restart download";
    redownloadButton.addEventListener("click", restartDownload);

    let deleteItemButtonB = document.createElement("BUTTON");
    deleteItemButtonB.className = "deleteItemButton";
    deleteItemButtonB.title = "Delete file from list";
    deleteItemButtonB.addEventListener("click", deleteDownloadItem);

    let itemButtonContainerD = document.createElement("DIV");
    itemButtonContainerD.className = "itemButtonContainerD";
    itemButtonContainerD.appendChild(redownloadButton);
    itemButtonContainerD.appendChild(deleteItemButtonB);

    let itemButtonContainer = document.createElement("DIV");
    itemButtonContainer.className = "itemButtonContainer";
    itemButtonContainer.appendChild(itemButtonContainerA);
    itemButtonContainer.appendChild(itemButtonContainerB);
    itemButtonContainer.appendChild(itemButtonContainerC);
    itemButtonContainer.appendChild(itemButtonContainerD);

    let progressBar = document.createElement("PROGRESS");
    progressBar.className = "progressBar";
    progressBar.max = "100";
    progressBar.value = "0";

    if (state.localeCompare("Downloading") == 0) {
        itemButtonContainerA.style.display = "flex";
        itemButtonContainerB.style.display = "none";
        itemButtonContainerC.style.display = "none";
        itemButtonContainerD.style.display = "none";
        itemSize.textContent = downloadSize + "/" + size;
    } else if (state.localeCompare("Paused") == 0) {
        itemButtonContainerA.style.display = "none";
        itemButtonContainerB.style.display = "none";
        itemButtonContainerC.style.display = "flex";
        itemButtonContainerD.style.display = "none";
        itemSize.textContent = downloadSize + "/" + size;
        progressBar.style.visibility = "hidden";
    } else if (state.localeCompare("Failed") == 0) {
        itemButtonContainerA.style.display = "none";
        itemButtonContainerB.style.display = "none";
        itemButtonContainerC.style.display = "none";
        itemButtonContainerD.style.display = "flex";
        itemSize.textContent = size;
        progressBar.style.visibility = "hidden";
    } else if (state.localeCompare("Complete") == 0) {
        itemButtonContainerA.style.display = "none";
        itemButtonContainerB.style.display = "flex";
        itemButtonContainerC.style.display = "none";
        itemButtonContainerD.style.display = "none";
        itemSize.textContent = size;
        progressBar.style.visibility = "hidden";
    }

    let itemControls = document.createElement("DIV");
    itemControls.className = "itemControls";
    itemControls.appendChild(progressBar);
    itemControls.appendChild(itemButtonContainer);

    let itemData = document.createElement("DIV");
    itemData.className = "itemData";
    itemData.appendChild(itemTitle);
    itemData.appendChild(itemUrl);
    itemData.appendChild(itemTransferData);
    itemData.appendChild(itemControls);

    let downloadItem = document.createElement("DIV");
    downloadItem.className = "item";
    downloadItem.dataset.downloadID = index;
    downloadItem.appendChild(iconSection);
    downloadItem.appendChild(itemData);

    return downloadItem;
}

function restartDownload(clickEvent) {
    let restartButton = clickEvent.target;

    let containerParent = restartButton.parentElement.parentElement;
    containerParent.childNodes.item(0).style.display = "flex";
    containerParent.childNodes.item(1).style.display = "none";
    containerParent.childNodes.item(2).style.display = "none";
    containerParent.childNodes.item(3).style.display = "none";

    let downloadItem = restartButton.parentElement.parentElement.parentElement.parentElement.parentElement;
    downloadItem.childNodes.item(1).childNodes.item(3).childNodes.item(0).style.visibility = "visible";
    downloadItem.childNodes.item(0).childNodes.item(0).src = "qrc:/web applications/images/activeDownload.png";
    downloadsCore.restartDownload(downloadItem.dataset.downloadID);
}

function deleteDownloadItem(clickEvent) {
    let deleteButton = clickEvent.target;
    let downloadItem = deleteButton.parentElement.parentElement.parentElement.parentElement.parentElement;
    let isGroupRemoving = false;

    if (downloadItem.previousElementSibling != null) {
        if (downloadItem.previousElementSibling.className.localeCompare("visitedDate") == 0) {
            if (downloadItem.nextElementSibling != null) {
                if (downloadItem.nextElementSibling.className.localeCompare("visitedDate") == 0) { isGroupRemoving = true; }
            } else { isGroupRemoving = true; }
        }
    }

    if (isGroupRemoving) {
        buttonTargetParent = downloadItem.previousElementSibling;
        deleteDownloadsDate();
    } else {
        downloadsCore.deleteDownloadItem(downloadItem.dataset.downloadID);
        downloadItem.classList.add("hide");
        setTimeout(function() { downloadItem.remove(); }, 121);
    }
}

function deleteDate(index) {
    let browsingDateArray = document.getElementsByClassName("downloadedDate");
    for (let i = 0; i < browsingDateArray.length; i++) {
        if (browsingDateArray.item(i).dataset.dateIndex.localeCompare(index) == 0) {
            browsingDateArray.item(i).classList.add("hide");
            setTimeout(function() { browsingDateArray.item(i).remove(); }, 130);
            break;
        }
    }
}

function deleteDownloadsDate() {
    closeDialogBoxTwo();

    let browsingDate = buttonTargetParent;
    let nextBrowsingDate;
    let browsingDateArray = document.getElementsByClassName("downloadedDate");

    for (let i = 0; i < browsingDateArray.length; i++) {
        if ((browsingDateArray[i] === browsingDate) && (i != (browsingDateArray.length - 1))) {
            nextBrowsingDate = browsingDateArray[i + 1];
            break;
        }
    }

    if (nextBrowsingDate != null) {
        let itemToDelete = document.getElementsByClassName("item");
        let rect1 = browsingDate.getBoundingClientRect();
        let rect2 = nextBrowsingDate.getBoundingClientRect();

        for (let i = 0; i < itemToDelete.length; i++) {
            let rect3 = itemToDelete[i].getBoundingClientRect();
            if ((rect1.bottom < rect3.top) && (rect2.top > rect3.bottom)) {
                downloadsCore.deleteDownloadItem(itemToDelete.item(i).dataset.downloadID);
                itemToDelete.item(i).remove();
                i--;
            }
        }

        browsingDate.classList.add("hide");
        setTimeout(function() { browsingDate.remove(); }, 130);
    } else {
        let itemToDelete = document.getElementsByClassName("item");
        let rect1 = browsingDate.getBoundingClientRect();

        for (let i = 0; i < itemToDelete.length; i++) {
            let rect3 = itemToDelete[i].getBoundingClientRect();
            if (rect1.bottom < rect3.top) {
                downloadsCore.deleteDownloadItem(itemToDelete.item(i).dataset.downloadID);
                itemToDelete.item(i).remove();
                i--;
            }
        }

        browsingDate.classList.add("hide");
        setTimeout(function() { browsingDate.remove(); }, 130);
    }
    buttonTargetParent = null;
}

function updateDownloadsList(index, title, url, size, downloadSize, icon, state) {
    let downloadItem = createDownloadItem(index, title, url, size, downloadSize, icon, state);
    let date = document.getElementsByClassName("downloadedDate");
    date.item(0).insertAdjacentElement("afterend", downloadItem);
}

function pauseDownload(clickEvent) {
    let pauseButton = clickEvent.target;

    let containerParent = pauseButton.parentElement.parentElement;
    containerParent.childNodes.item(0).style.display = "none";
    containerParent.childNodes.item(1).style.display = "none";
    containerParent.childNodes.item(2).style.display = "flex";
    containerParent.childNodes.item(3).style.display = "none";

    let downloadItem = pauseButton.parentElement.parentElement.parentElement.parentElement.parentElement;
    downloadItem.childNodes.item(1).childNodes.item(2).childNodes.item(1).textContent = "Paused";
    downloadItem.childNodes.item(1).childNodes.item(3).childNodes.item(0).style.visibility = "hidden";
    downloadItem.childNodes.item(0).childNodes.item(0).src = "qrc:/web applications/images/pausedDownload.png";
    downloadsCore.pauseDownload(downloadItem.dataset.downloadID);
}

function pauseItem(index) {
    let items = document.getElementsByClassName("item");

    for (let i = 0; i < items.length; i++) {
        if (items.item(i).dataset.downloadID.localeCompare(index) == 0) {
            items.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(1).textContent = "Paused";
            items.item(i).childNodes.item(1).childNodes.item(3).childNodes.item(0).style.visibility = "hidden";

            let containerParent = items.item(i).childNodes.item(1).childNodes.item(3).childNodes.item(1);
            containerParent.childNodes.item(0).style.display = "none";
            containerParent.childNodes.item(1).style.display = "none";
            containerParent.childNodes.item(2).style.display = "flex";
            containerParent.childNodes.item(3).style.display = "none";
            break;
        }
    }
}

function stopDownload(clickEvent) {
    let stopButton = clickEvent.target;

    let containerParent = stopButton.parentElement.parentElement;
    containerParent.childNodes.item(0).style.display = "none";
    containerParent.childNodes.item(1).style.display = "none";
    containerParent.childNodes.item(2).style.display = "none";
    containerParent.childNodes.item(3).style.display = "flex";

    let downloadItem = stopButton.parentElement.parentElement.parentElement.parentElement.parentElement;
    downloadItem.childNodes.item(1).childNodes.item(2).childNodes.item(1).textContent = "Failed";
    downloadItem.childNodes.item(1).childNodes.item(3).childNodes.item(0).style.visibility = "hidden";
    downloadItem.childNodes.item(0).childNodes.item(0).src = "qrc:/web applications/images/failedDownload.png";
    downloadsCore.stopDownload(downloadItem.dataset.downloadID);
}

function stopItem(index) {
    let items = document.getElementsByClassName("item");

    for (let i = 0; i < items.length; i++) {
        if (items.item(i).dataset.downloadID.localeCompare(index) == 0) {
            items.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(1).textContent = "Failed";
            items.item(i).childNodes.item(1).childNodes.item(3).childNodes.item(0).style.visibility = "hidden";

            let containerParent = items.item(i).childNodes.item(1).childNodes.item(3).childNodes.item(1);
            containerParent.childNodes.item(0).style.display = "none";
            containerParent.childNodes.item(1).style.display = "none";
            containerParent.childNodes.item(2).style.display = "none";
            containerParent.childNodes.item(3).style.display = "flex";
            break;
        }
    }
}

function openDownloadLocation(clickEvent) {
    let openDLButton = clickEvent.target;
    let downloadItem = openDLButton.parentElement.parentElement.parentElement.parentElement.parentElement;
    downloadsCore.openDownloadLocation(downloadItem.dataset.downloadID);
}

function resumeDownload(clickEvent) {
    let resumeButton = clickEvent.target;

    let containerParent = resumeButton.parentElement.parentElement;
    containerParent.childNodes.item(0).style.display = "flex";
    containerParent.childNodes.item(1).style.display = "none";
    containerParent.childNodes.item(2).style.display = "none";
    containerParent.childNodes.item(3).style.display = "none";

    let downloadItem = resumeButton.parentElement.parentElement.parentElement.parentElement.parentElement;
    downloadItem.childNodes.item(1).childNodes.item(3).childNodes.item(0).style.visibility = "visible";
    downloadItem.childNodes.item(0).childNodes.item(0).src = "qrc:/web applications/images/activeDownload.png";
    downloadsCore.resumeDownload(downloadItem.dataset.downloadID);
}

function resumeItem(index) {
    let items = document.getElementsByClassName("item");

    for (let i = 0; i < items.length; i++) {
        if (items.item(i).dataset.downloadID.localeCompare(index) == 0) {
            items.item(i).childNodes.item(1).childNodes.item(3).childNodes.item(0).style.visibility = "visible";

            let containerParent = items.item(i).childNodes.item(1).childNodes.item(3).childNodes.item(1);
            containerParent.childNodes.item(0).style.display = "flex";
            containerParent.childNodes.item(1).style.display = "none";
            containerParent.childNodes.item(2).style.display = "none";
            containerParent.childNodes.item(3).style.display = "none";
            break;
        }
    }
}

function openDownloadsFolder() {
    downloadsCore.openDownloadsFolder();
}

function clearDownloadsList() {
    closeDialogBoxOne();
    let itemList = document.getElementById("itemList");
    itemList.innerHTML = "";
    loadEmptyDownloadView(true);
    downloadsCore.clearDownloadsList();
}

function downloadFinished(index, size) {
    let item = document.getElementsByClassName("item");
    for (let i = 0; i < item.length; i++) {
        if (item.item(i).dataset.downloadID.localeCompare(index) == 0) {
            let containerA = document.getElementsByClassName("itemButtonContainerA");
            containerA.item(i).style.display = "none";

            let containerB = document.getElementsByClassName("itemButtonContainerB");
            containerB.item(i).style.display = "flex";

            let containerC = document.getElementsByClassName("itemButtonContainerC");
            containerC.item(i).style.display = "none";

            let containerD = document.getElementsByClassName("itemButtonContainerD");
            containerD.item(i).style.display = "none";

            let progressBar = document.getElementsByClassName("progressBar");
            progressBar.item(i).style.visibility = "hidden";

            item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(0).textContent = size;
            item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(1).textContent = "Complete";
            item.item(i).childNodes.item(0).childNodes.item(0).src = "qrc:/web applications/images/completeDownload.png";
            break;
        }
    }
}

function loadEmptyDownloadView(state) {
    let empty = document.getElementById("emptyView");
    let itemList = document.getElementById("itemList");
    let noresult = document.getElementById("noResult");
    noresult.style.display = "none";

    if (state) {
        empty.style.display = "flex";
        itemList.style.display = "none";
    } else {
        empty.style.display = "none";
        itemList.style.display = "block";
    }
}

function updateTransferRate(index, rate, size, percentage) {
    let item = document.getElementsByClassName("item");
    for (let i = 0; i < item.length; i++) {
        if (item.item(i).dataset.downloadID.localeCompare(index) == 0) {
            item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(0).textContent = size;
            item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(1).textContent = rate;
            item.item(i).childNodes.item(1).childNodes.item(3).childNodes.item(0).value = percentage;
            break;
        }
    }
}

function noDownloadResult() {
    let itemList = document.getElementById("itemList");
    let emptyView = document.getElementById("emptyView");
    let noResult = document.getElementById("noResult");
    itemList.style.display = "none";
    emptyView.style.display = "none";
    noResult.style.display = "flex";
}