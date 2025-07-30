"use strict";
let downloadsWidgetCore = null;

function setupConnector() {
    new QWebChannel(qt.webChannelTransport, function(ololanChannel) {
        downloadsWidgetCore = ololanChannel.objects.widgetCore;
    });
}

function contentScrolled() {
    let controlBar = document.getElementById("controlBar");
    let itemView = document.getElementById("itemView");

    if (itemView.scrollTop > 12) {
        controlBar.style.borderBottom = "1px solid #efeff1";
        controlBar.style.boxShadow = "0px 1px 2px 0px #d1d1d1";
    } else {
        controlBar.style.borderBottom = "1px solid transparent";
        controlBar.style.boxShadow = "none";
    }
}

function loadDownloadItem(index, title, icon, size, state) {
    let item = createDownloadItem(index, title, icon, size, state);
    let itemList = document.getElementById("itemList");
    itemList.appendChild(item);
}

function createDownloadItem(index, title, icon, size, state) {
    let itemIcon = document.createElement("IMG");
    itemIcon.className = "itemIcon";
    itemIcon.src = icon;

    let iconSection = document.createElement("DIV");
    iconSection.className = "iconSection";
    iconSection.appendChild(itemIcon);

    let itemTitle = document.createElement("DIV");
    itemTitle.className = "itemTitle";
    itemTitle.textContent = title;

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

    let resumeButton = document.createElement("BUTTON");
    resumeButton.className = "resumeButton";
    resumeButton.title = "Resume download";
    resumeButton.addEventListener("click", resumeDownload);

    let stopButtonB = document.createElement("BUTTON");
    stopButtonB.className = "stopButton";
    stopButtonB.title = "Stop downloading";
    stopButtonB.addEventListener("click", stopDownload);

    let itemButtonContainerB = document.createElement("DIV");
    itemButtonContainerB.className = "itemButtonContainerB";
    itemButtonContainerB.appendChild(resumeButton);
    itemButtonContainerB.appendChild(stopButtonB);

    let itemButtonContainer = document.createElement("DIV");
    itemButtonContainer.className = "itemButtonContainer";
    itemButtonContainer.appendChild(itemButtonContainerA);
    itemButtonContainer.appendChild(itemButtonContainerB);

    let progressBar = document.createElement("PROGRESS");
    progressBar.className = "progressBar";
    progressBar.max = "100";
    progressBar.value = "0";

    if (state.localeCompare("Downloading") == 0) {
        itemButtonContainerA.style.display = "flex";
        itemButtonContainerB.style.display = "none";
        itemSize.textContent = size;
    } else if (state.localeCompare("Paused") == 0) {
        itemButtonContainerA.style.display = "none";
        itemButtonContainerB.style.display = "flex";
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
    itemData.appendChild(itemTransferData);
    itemData.appendChild(itemControls);

    let downloadItem = document.createElement("DIV");
    downloadItem.className = "item";
    downloadItem.dataset.downloadID = index;
    downloadItem.appendChild(iconSection);
    downloadItem.appendChild(itemData);

    return downloadItem;
}

function updateTransferRate(index, rate, size, percentage) {
    let item = document.getElementsByClassName("item");
    for (let i = 0; i < item.length; i++) {
        if (item.item(i).dataset.downloadID.localeCompare(index) == 0) {
            item.item(i).childNodes.item(1).childNodes.item(1).childNodes.item(0).textContent = size;
            item.item(i).childNodes.item(1).childNodes.item(1).childNodes.item(1).textContent = rate;
            item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(0).value = percentage;

            if (item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(0).style.visibility.localeCompare("visible") != 0) {
                item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(0).style.visibility = "visible";
                item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(1).childNodes.item(0).style.display = "flex";
                item.item(i).childNodes.item(1).childNodes.item(2).childNodes.item(1).childNodes.item(1).style.display = "none";
            }
            break;
        }
    }
}

function loadEmptyWidgetView(state) {
    let empty = document.getElementById("emptyView");
    let itemList = document.getElementById("itemList");
    let pFooter = document.getElementById("pageFooter");
    let pBody = document.getElementById("pageBody");

    if (state) {
        empty.style.display = "flex";
        itemList.style.display = "none";
        pFooter.style.display = "none";
        pBody.style.bottom = "0px";
    } else {
        empty.style.display = "none";
        itemList.style.display = "block";
        pFooter.style.display = "flex";
        pBody.style.bottom = "51px";
    }
}

function deleteDownloadItem(index) {
    let downloadItemList = document.getElementsByClassName("item");
    for (let i = 0; i < downloadItemList.length; i++) {
        if (downloadItemList.item(i).dataset.downloadID.localeCompare(index) == 0) {
            downloadItemList.item(i).classList.add("hide");
            setTimeout(function() { downloadItemList.item(i).remove(); }, 121);
            break;
        }
    }

    if (downloadItemList.length < 1) {
        loadEmptyWidgetView(true);
    }
}

function clearDownloadsList() {
    let itemList = document.getElementById("itemList");
    itemList.innerHTML = "";
    loadEmptyWidgetView(true);
}

function pauseDownload(clickEvent) {
    let pauseButton = clickEvent.target;

    let containerParent = pauseButton.parentElement.parentElement;
    containerParent.childNodes.item(0).style.display = "none";
    containerParent.childNodes.item(1).style.display = "flex";

    let downloadItem = pauseButton.parentElement.parentElement.parentElement.parentElement;
    downloadItem.childNodes.item(1).childNodes.item(1).textContent = "Paused";
    downloadItem.childNodes.item(2).childNodes.item(0).style.visibility = "hidden";
    downloadItem.parentElement.childNodes.item(0).childNodes.item(0).src = "qrc:/web applications/images/pausedDownload.png";
    downloadsWidgetCore.pauseDownload(downloadItem.parentElement.dataset.downloadID);
}

function stopDownload(clickEvent) {
    let stopButton = clickEvent.target;

    let downloadItem = stopButton.parentElement.parentElement.parentElement.parentElement;
    downloadItem.childNodes.item(1).childNodes.item(1).textContent = "Failed";
    downloadItem.childNodes.item(2).childNodes.item(0).style.visibility = "hidden";
    downloadItem.parentElement.childNodes.item(0).childNodes.item(0).src = "qrc:/web applications/images/failedDownload.png";
    downloadsWidgetCore.stopDownload(downloadItem.parentElement.dataset.downloadID);
}

function resumeDownload(clickEvent) {
    let resumeButton = clickEvent.target;

    let containerParent = resumeButton.parentElement.parentElement;
    containerParent.childNodes.item(0).style.display = "flex";
    containerParent.childNodes.item(1).style.display = "none";

    let downloadItem = resumeButton.parentElement.parentElement.parentElement.parentElement;
    downloadItem.childNodes.item(2).childNodes.item(0).style.visibility = "visible";
    downloadItem.parentElement.childNodes.item(0).childNodes.item(0).src = "qrc:/web applications/images/activeDownload.png";
    downloadsWidgetCore.resumeDownload(downloadItem.parentElement.dataset.downloadID);
}

function openManager() {
    downloadsWidgetCore.openManager();
}

function setLightGrayTheme() {
    let itemList = document.getElementsByClassName("item");
    for (let i = 0; i < itemList.length; i++) {
        itemList.item(i).classList.remove("lightturquoise");
        itemList.item(i).classList.remove("lightbrown");
        itemList.item(i).classList.remove("darkgray");
        itemList.item(i).classList.remove("darkturquoise");
        itemList.item(i).classList.remove("darkbrown");
        itemList.item(i).classList.remove("private");
        itemList.item(i).classList.add("lightgray");
    }

    let progressList = document.getElementsByClassName("progressBar");
    for (let i = 0; i < progressList.length; i++) {
        progressList.item(i).classList.remove("lightturquoise");
        progressList.item(i).classList.remove("lightbrown");
        progressList.item(i).classList.remove("darkgray");
        progressList.item(i).classList.remove("darkturquoise");
        progressList.item(i).classList.remove("darkbrown");
        progressList.item(i).classList.remove("private");
        progressList.item(i).classList.add("lightgray");
    }

    let pauseList = document.getElementsByClassName("pauseButton");
    for (let i = 0; i < pauseList.length; i++) {
        pauseList.item(i).classList.remove("controllightturquoise");
        pauseList.item(i).classList.remove("controllightbrown");
        pauseList.item(i).classList.remove("controldarkgray");
        pauseList.item(i).classList.remove("controldarkturquoise");
        pauseList.item(i).classList.remove("controldarkbrown");
        pauseList.item(i).classList.remove("controlprivate");
        pauseList.item(i).classList.add("controllightgray");
    }

    let resumeList = document.getElementsByClassName("resumeButton");
    for (let i = 0; i < resumeList.length; i++) {
        resumeList.item(i).classList.remove("controllightturquoise");
        resumeList.item(i).classList.remove("controllightbrown");
        resumeList.item(i).classList.remove("controldarkgray");
        resumeList.item(i).classList.remove("controldarkturquoise");
        resumeList.item(i).classList.remove("controldarkbrown");
        resumeList.item(i).classList.remove("controlprivate");
        resumeList.item(i).classList.add("controllightgray");
    }

    let stopList = document.getElementsByClassName("stopButton");
    for (let i = 0; i < stopList.length; i++) {
        stopList.item(i).classList.remove("controllightturquoise");
        stopList.item(i).classList.remove("controllightbrown");
        stopList.item(i).classList.remove("controldarkgray");
        stopList.item(i).classList.remove("controldarkturquoise");
        stopList.item(i).classList.remove("controldarkbrown");
        stopList.item(i).classList.remove("controlprivate");
        stopList.item(i).classList.add("controllightgray");
    }

    let titleList = document.getElementsByClassName("itemTitle");
    for (let i = 0; i < titleList.length; i++) {
        titleList.item(i).style.color = "#656565";
    }

    let sizeList = document.getElementsByClassName("itemSize");
    for (let i = 0; i < sizeList.length; i++) {
        sizeList.item(i).style.color = "#656565";
    }

    let rateList = document.getElementsByClassName("itemTransferRate");
    for (let i = 0; i < rateList.length; i++) {
        rateList.item(i).style.color = "#656565";
    }

    document.body.style.backgroundColor = "white";
    document.getElementById("pageToolBar").style.backgroundColor = "white";
    document.getElementById("pageBody").style.backgroundColor = "white";
    document.getElementById("eViewMessage").style.color = "#656565";
    document.getElementById("emptyView").style.color = "#656565";
    document.getElementById("pageTitle").style.color = "#68e7f0";
    document.getElementById("downloadsManagerButton").classList.remove("lightturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("lightbrown");
    document.getElementById("downloadsManagerButton").classList.remove("darkgray");
    document.getElementById("downloadsManagerButton").classList.remove("darkturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("darkbrown");
    document.getElementById("downloadsManagerButton").classList.remove("private");
    document.getElementById("downloadsManagerButton").classList.add("lightgray");
}

function setLightTurquoiseTheme() {
    let itemList = document.getElementsByClassName("item");
    for (let i = 0; i < itemList.length; i++) {
        itemList.item(i).classList.remove("lightgray");
        itemList.item(i).classList.remove("lightbrown");
        itemList.item(i).classList.remove("darkgray");
        itemList.item(i).classList.remove("darkturquoise");
        itemList.item(i).classList.remove("darkbrown");
        itemList.item(i).classList.remove("private");
        itemList.item(i).classList.add("lightturquoise");
    }

    let progressList = document.getElementsByClassName("progressBar");
    for (let i = 0; i < progressList.length; i++) {
        progressList.item(i).classList.remove("lightgray");
        progressList.item(i).classList.remove("lightbrown");
        progressList.item(i).classList.remove("darkgray");
        progressList.item(i).classList.remove("darkturquoise");
        progressList.item(i).classList.remove("darkbrown");
        progressList.item(i).classList.remove("private");
        progressList.item(i).classList.add("lightturquoise");
    }

    let pauseList = document.getElementsByClassName("pauseButton");
    for (let i = 0; i < pauseList.length; i++) {
        pauseList.item(i).classList.remove("controllightgray");
        pauseList.item(i).classList.remove("controllightbrown");
        pauseList.item(i).classList.remove("controldarkgray");
        pauseList.item(i).classList.remove("controldarkturquoise");
        pauseList.item(i).classList.remove("controldarkbrown");
        pauseList.item(i).classList.remove("controlprivate");
        pauseList.item(i).classList.add("controllightturquoise");
    }

    let resumeList = document.getElementsByClassName("resumeButton");
    for (let i = 0; i < resumeList.length; i++) {
        resumeList.item(i).classList.remove("controllightgray");
        resumeList.item(i).classList.remove("controllightbrown");
        resumeList.item(i).classList.remove("controldarkgray");
        resumeList.item(i).classList.remove("controldarkturquoise");
        resumeList.item(i).classList.remove("controldarkbrown");
        resumeList.item(i).classList.remove("controlprivate");
        resumeList.item(i).classList.add("controllightturquoise");
    }

    let stopList = document.getElementsByClassName("stopButton");
    for (let i = 0; i < stopList.length; i++) {
        stopList.item(i).classList.remove("controllightgray");
        stopList.item(i).classList.remove("controllightbrown");
        stopList.item(i).classList.remove("controldarkgray");
        stopList.item(i).classList.remove("controldarkturquoise");
        stopList.item(i).classList.remove("controldarkbrown");
        stopList.item(i).classList.remove("controlprivate");
        stopList.item(i).classList.add("controllightturquoise");
    }

    let titleList = document.getElementsByClassName("itemTitle");
    for (let i = 0; i < titleList.length; i++) {
        titleList.item(i).style.color = "#656565";
    }

    let sizeList = document.getElementsByClassName("itemSize");
    for (let i = 0; i < sizeList.length; i++) {
        sizeList.item(i).style.color = "#656565";
    }

    let rateList = document.getElementsByClassName("itemTransferRate");
    for (let i = 0; i < rateList.length; i++) {
        rateList.item(i).style.color = "#656565";
    }

    document.body.style.backgroundColor = "white";
    document.getElementById("pageToolBar").style.backgroundColor = "white";
    document.getElementById("pageBody").style.backgroundColor = "white";
    document.getElementById("eViewMessage").style.color = "#656565";
    document.getElementById("emptyView").style.color = "#656565";
    document.getElementById("pageTitle").style.color = "#68e7f0";
    document.getElementById("downloadsManagerButton").classList.remove("lightgray");
    document.getElementById("downloadsManagerButton").classList.remove("lightbrown");
    document.getElementById("downloadsManagerButton").classList.remove("darkgray");
    document.getElementById("downloadsManagerButton").classList.remove("darkturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("darkbrown");
    document.getElementById("downloadsManagerButton").classList.remove("private");
    document.getElementById("downloadsManagerButton").classList.add("lightturquoise");
}

function setLightBrownTheme() {
    let itemList = document.getElementsByClassName("item");
    for (let i = 0; i < itemList.length; i++) {
        itemList.item(i).classList.remove("lightgray");
        itemList.item(i).classList.remove("lightturquoise");
        itemList.item(i).classList.remove("darkgray");
        itemList.item(i).classList.remove("darkturquoise");
        itemList.item(i).classList.remove("darkbrown");
        itemList.item(i).classList.remove("private");
        itemList.item(i).classList.add("lightbrown");
    }

    let progressList = document.getElementsByClassName("progressBar");
    for (let i = 0; i < progressList.length; i++) {
        progressList.item(i).classList.remove("lightgray");
        progressList.item(i).classList.remove("lightturquoise");
        progressList.item(i).classList.remove("darkgray");
        progressList.item(i).classList.remove("darkturquoise");
        progressList.item(i).classList.remove("darkbrown");
        progressList.item(i).classList.remove("private");
        progressList.item(i).classList.add("lightbrown");
    }

    let pauseList = document.getElementsByClassName("pauseButton");
    for (let i = 0; i < pauseList.length; i++) {
        pauseList.item(i).classList.remove("controllightgray");
        pauseList.item(i).classList.remove("controllightturquoise");
        pauseList.item(i).classList.remove("controldarkgray");
        pauseList.item(i).classList.remove("controldarkturquoise");
        pauseList.item(i).classList.remove("controldarkbrown");
        pauseList.item(i).classList.remove("controlprivate");
        pauseList.item(i).classList.add("controllightbrown");
    }

    let resumeList = document.getElementsByClassName("resumeButton");
    for (let i = 0; i < resumeList.length; i++) {
        resumeList.item(i).classList.remove("controllightgray");
        resumeList.item(i).classList.remove("controllightturquoise");
        resumeList.item(i).classList.remove("controldarkgray");
        resumeList.item(i).classList.remove("controldarkturquoise");
        resumeList.item(i).classList.remove("controldarkbrown");
        resumeList.item(i).classList.remove("controlprivate");
        resumeList.item(i).classList.add("controllightbrown");
    }

    let stopList = document.getElementsByClassName("stopButton");
    for (let i = 0; i < stopList.length; i++) {
        stopList.item(i).classList.remove("controllightgray");
        stopList.item(i).classList.remove("controllightturquoise");
        stopList.item(i).classList.remove("controldarkgray");
        stopList.item(i).classList.remove("controldarkturquoise");
        stopList.item(i).classList.remove("controldarkbrown");
        stopList.item(i).classList.remove("controlprivate");
        stopList.item(i).classList.add("controllightbrown");
    }

    let titleList = document.getElementsByClassName("itemTitle");
    for (let i = 0; i < titleList.length; i++) {
        titleList.item(i).style.color = "#656565";
    }

    let sizeList = document.getElementsByClassName("itemSize");
    for (let i = 0; i < sizeList.length; i++) {
        sizeList.item(i).style.color = "#656565";
    }

    let rateList = document.getElementsByClassName("itemTransferRate");
    for (let i = 0; i < rateList.length; i++) {
        rateList.item(i).style.color = "#656565";
    }

    document.body.style.backgroundColor = "white";
    document.getElementById("pageToolBar").style.backgroundColor = "white";
    document.getElementById("pageBody").style.backgroundColor = "white";
    document.getElementById("eViewMessage").style.color = "#656565";
    document.getElementById("emptyView").style.color = "#656565";
    document.getElementById("pageTitle").style.color = "#ffa477";
    document.getElementById("downloadsManagerButton").classList.remove("lightturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("lightgray");
    document.getElementById("downloadsManagerButton").classList.remove("darkgray");
    document.getElementById("downloadsManagerButton").classList.remove("darkturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("darkbrown");
    document.getElementById("downloadsManagerButton").classList.remove("private");
    document.getElementById("downloadsManagerButton").classList.add("lightbrown");
}

function setDarkGrayTheme() {
    let itemList = document.getElementsByClassName("item");
    for (let i = 0; i < itemList.length; i++) {
        itemList.item(i).classList.remove("lightgray");
        itemList.item(i).classList.remove("lightturquoise");
        itemList.item(i).classList.remove("lightbrown");
        itemList.item(i).classList.remove("darkturquoise");
        itemList.item(i).classList.remove("darkbrown");
        itemList.item(i).classList.remove("private");
        itemList.item(i).classList.add("darkgray");
    }

    let progressList = document.getElementsByClassName("progressBar");
    for (let i = 0; i < progressList.length; i++) {
        progressList.item(i).classList.remove("lightgray");
        progressList.item(i).classList.remove("lightturquoise");
        progressList.item(i).classList.remove("lightbrown");
        progressList.item(i).classList.remove("darkturquoise");
        progressList.item(i).classList.remove("darkbrown");
        progressList.item(i).classList.remove("private");
        progressList.item(i).classList.add("darkgray");
    }

    let pauseList = document.getElementsByClassName("pauseButton");
    for (let i = 0; i < pauseList.length; i++) {
        pauseList.item(i).classList.remove("controllightgray");
        pauseList.item(i).classList.remove("controllightturquoise");
        pauseList.item(i).classList.remove("controllightbrown");
        pauseList.item(i).classList.remove("controldarkturquoise");
        pauseList.item(i).classList.remove("controldarkbrown");
        pauseList.item(i).classList.remove("controlprivate");
        pauseList.item(i).classList.add("controldarkgray");
    }

    let resumeList = document.getElementsByClassName("resumeButton");
    for (let i = 0; i < resumeList.length; i++) {
        resumeList.item(i).classList.remove("controllightgray");
        resumeList.item(i).classList.remove("controllightturquoise");
        resumeList.item(i).classList.remove("controllightbrown");
        resumeList.item(i).classList.remove("controldarkturquoise");
        resumeList.item(i).classList.remove("controldarkbrown");
        resumeList.item(i).classList.remove("controlprivate");
        resumeList.item(i).classList.add("controldarkgray");
    }

    let stopList = document.getElementsByClassName("stopButton");
    for (let i = 0; i < stopList.length; i++) {
        stopList.item(i).classList.remove("controllightgray");
        stopList.item(i).classList.remove("controllightturquoise");
        stopList.item(i).classList.remove("controllightbrown");
        stopList.item(i).classList.remove("controldarkturquoise");
        stopList.item(i).classList.remove("controldarkbrown");
        stopList.item(i).classList.remove("controlprivate");
        stopList.item(i).classList.add("controldarkgray");
    }

    let titleList = document.getElementsByClassName("itemTitle");
    for (let i = 0; i < titleList.length; i++) {
        titleList.item(i).style.color = "white";
    }

    let sizeList = document.getElementsByClassName("itemSize");
    for (let i = 0; i < sizeList.length; i++) {
        sizeList.item(i).style.color = "white";
    }

    let rateList = document.getElementsByClassName("itemTransferRate");
    for (let i = 0; i < rateList.length; i++) {
        rateList.item(i).style.color = "white";
    }

    document.body.style.backgroundColor = "#434344";
    document.getElementById("pageToolBar").style.backgroundColor = "#434344";
    document.getElementById("pageBody").style.backgroundColor = "#434344";
    document.getElementById("eViewMessage").style.color = "white";
    document.getElementById("emptyView").style.color = "white";
    document.getElementById("pageTitle").style.color = "white";
    document.getElementById("downloadsManagerButton").classList.remove("lightturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("lightbrown");
    document.getElementById("downloadsManagerButton").classList.remove("lightgray");
    document.getElementById("downloadsManagerButton").classList.remove("darkturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("darkbrown");
    document.getElementById("downloadsManagerButton").classList.remove("private");
    document.getElementById("downloadsManagerButton").classList.add("darkgray");
}

function setDarkTurquoiseTheme() {
    let itemList = document.getElementsByClassName("item");
    for (let i = 0; i < itemList.length; i++) {
        itemList.item(i).classList.remove("lightgray");
        itemList.item(i).classList.remove("lightturquoise");
        itemList.item(i).classList.remove("lightbrown");
        itemList.item(i).classList.remove("darkgray");
        itemList.item(i).classList.remove("darkbrown");
        itemList.item(i).classList.remove("private");
        itemList.item(i).classList.add("darkturquoise");
    }

    let progressList = document.getElementsByClassName("progressBar");
    for (let i = 0; i < progressList.length; i++) {
        progressList.item(i).classList.remove("lightgray");
        progressList.item(i).classList.remove("lightturquoise");
        progressList.item(i).classList.remove("lightbrown");
        progressList.item(i).classList.remove("darkgray");
        progressList.item(i).classList.remove("darkbrown");
        progressList.item(i).classList.remove("private");
        progressList.item(i).classList.add("darkturquoise");
    }

    let pauseList = document.getElementsByClassName("pauseButton");
    for (let i = 0; i < pauseList.length; i++) {
        pauseList.item(i).classList.remove("controllightgray");
        pauseList.item(i).classList.remove("controllightturquoise");
        pauseList.item(i).classList.remove("controllightbrown");
        pauseList.item(i).classList.remove("controldarkgray");
        pauseList.item(i).classList.remove("controldarkbrown");
        pauseList.item(i).classList.remove("controlprivate");
        pauseList.item(i).classList.add("controldarkturquoise");
    }

    let resumeList = document.getElementsByClassName("resumeButton");
    for (let i = 0; i < resumeList.length; i++) {
        resumeList.item(i).classList.remove("controllightgray");
        resumeList.item(i).classList.remove("controllightturquoise");
        resumeList.item(i).classList.remove("controllightbrown");
        resumeList.item(i).classList.remove("controldarkgray");
        resumeList.item(i).classList.remove("controldarkbrown");
        resumeList.item(i).classList.remove("controlprivate");
        resumeList.item(i).classList.add("controldarkturquoise");
    }

    let stopList = document.getElementsByClassName("stopButton");
    for (let i = 0; i < stopList.length; i++) {
        stopList.item(i).classList.remove("controllightgray");
        stopList.item(i).classList.remove("controllightturquoise");
        stopList.item(i).classList.remove("controllightbrown");
        stopList.item(i).classList.remove("controldarkgray");
        stopList.item(i).classList.remove("controldarkbrown");
        stopList.item(i).classList.remove("controlprivate");
        stopList.item(i).classList.add("controldarkturquoise");
    }

    let titleList = document.getElementsByClassName("itemTitle");
    for (let i = 0; i < titleList.length; i++) {
        titleList.item(i).style.color = "white";
    }

    let sizeList = document.getElementsByClassName("itemSize");
    for (let i = 0; i < sizeList.length; i++) {
        sizeList.item(i).style.color = "white";
    }

    let rateList = document.getElementsByClassName("itemTransferRate");
    for (let i = 0; i < rateList.length; i++) {
        rateList.item(i).style.color = "white";
    }

    document.body.style.backgroundColor = "#1e4446";
    document.getElementById("pageToolBar").style.backgroundColor = "#1e4446";
    document.getElementById("pageBody").style.backgroundColor = "#1e4446";
    document.getElementById("eViewMessage").style.color = "white";
    document.getElementById("emptyView").style.color = "white";
    document.getElementById("pageTitle").style.color = "white";
    document.getElementById("downloadsManagerButton").classList.remove("lightturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("lightbrown");
    document.getElementById("downloadsManagerButton").classList.remove("darkgray");
    document.getElementById("downloadsManagerButton").classList.remove("lightgray");
    document.getElementById("downloadsManagerButton").classList.remove("darkbrown");
    document.getElementById("downloadsManagerButton").classList.remove("private");
    document.getElementById("downloadsManagerButton").classList.add("darkturquoise");
}

function setDarkBrownTheme() {
    let itemList = document.getElementsByClassName("item");
    for (let i = 0; i < itemList.length; i++) {
        itemList.item(i).classList.remove("lightgray");
        itemList.item(i).classList.remove("lightturquoise");
        itemList.item(i).classList.remove("lightbrown");
        itemList.item(i).classList.remove("darkgray");
        itemList.item(i).classList.remove("darkturquoise");
        itemList.item(i).classList.remove("private");
        itemList.item(i).classList.add("darkbrown");
    }

    let progressList = document.getElementsByClassName("progressBar");
    for (let i = 0; i < progressList.length; i++) {
        progressList.item(i).classList.remove("lightgray");
        progressList.item(i).classList.remove("lightturquoise");
        progressList.item(i).classList.remove("lightbrown");
        progressList.item(i).classList.remove("darkgray");
        progressList.item(i).classList.remove("darkturquoise");
        progressList.item(i).classList.remove("private");
        progressList.item(i).classList.add("darkbrown");
    }

    let pauseList = document.getElementsByClassName("pauseButton");
    for (let i = 0; i < pauseList.length; i++) {
        pauseList.item(i).classList.remove("controllightgray");
        pauseList.item(i).classList.remove("controllightturquoise");
        pauseList.item(i).classList.remove("controllightbrown");
        pauseList.item(i).classList.remove("controldarkgray");
        pauseList.item(i).classList.remove("controldarkturquoise");
        pauseList.item(i).classList.remove("controlprivate");
        pauseList.item(i).classList.add("controldarkbrown");
    }

    let resumeList = document.getElementsByClassName("resumeButton");
    for (let i = 0; i < resumeList.length; i++) {
        resumeList.item(i).classList.remove("controllightgray");
        resumeList.item(i).classList.remove("controllightturquoise");
        resumeList.item(i).classList.remove("controllightbrown");
        resumeList.item(i).classList.remove("controldarkgray");
        resumeList.item(i).classList.remove("controldarkturquoise");
        resumeList.item(i).classList.remove("controlprivate");
        resumeList.item(i).classList.add("controldarkbrown");
    }

    let stopList = document.getElementsByClassName("stopButton");
    for (let i = 0; i < stopList.length; i++) {
        stopList.item(i).classList.remove("controllightgray");
        stopList.item(i).classList.remove("controllightturquoise");
        stopList.item(i).classList.remove("controllightbrown");
        stopList.item(i).classList.remove("controldarkgray");
        stopList.item(i).classList.remove("controldarkturquoise");
        stopList.item(i).classList.remove("controlprivate");
        stopList.item(i).classList.add("controldarkbrown");
    }

    let titleList = document.getElementsByClassName("itemTitle");
    for (let i = 0; i < titleList.length; i++) {
        titleList.item(i).style.color = "white";
    }

    let sizeList = document.getElementsByClassName("itemSize");
    for (let i = 0; i < sizeList.length; i++) {
        sizeList.item(i).style.color = "white";
    }

    let rateList = document.getElementsByClassName("itemTransferRate");
    for (let i = 0; i < rateList.length; i++) {
        rateList.item(i).style.color = "white";
    }

    document.body.style.backgroundColor = "#462c21";
    document.getElementById("pageToolBar").style.backgroundColor = "#462c21";
    document.getElementById("pageBody").style.backgroundColor = "#462c21";
    document.getElementById("eViewMessage").style.color = "white";
    document.getElementById("emptyView").style.color = "white";
    document.getElementById("pageTitle").style.color = "white";
    document.getElementById("downloadsManagerButton").classList.remove("lightturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("lightbrown");
    document.getElementById("downloadsManagerButton").classList.remove("darkgray");
    document.getElementById("downloadsManagerButton").classList.remove("darkturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("lightgray");
    document.getElementById("downloadsManagerButton").classList.remove("private");
    document.getElementById("downloadsManagerButton").classList.add("darkbrown");
}

function setPrivateTheme() {
    let itemList = document.getElementsByClassName("item");
    for (let i = 0; i < itemList.length; i++) {
        itemList.item(i).classList.remove("lightgray");
        itemList.item(i).classList.remove("lightturquoise");
        itemList.item(i).classList.remove("lightbrown");
        itemList.item(i).classList.remove("darkgray");
        itemList.item(i).classList.remove("darkturquoise");
        itemList.item(i).classList.remove("darkbrown");
        itemList.item(i).classList.add("private");
    }

    let progressList = document.getElementsByClassName("progressBar");
    for (let i = 0; i < progressList.length; i++) {
        progressList.item(i).classList.remove("lightgray");
        progressList.item(i).classList.remove("lightturquoise");
        progressList.item(i).classList.remove("lightbrown");
        progressList.item(i).classList.remove("darkgray");
        progressList.item(i).classList.remove("darkturquoise");
        progressList.item(i).classList.remove("darkbrown");
        progressList.item(i).classList.add("private");
    }

    let pauseList = document.getElementsByClassName("pauseButton");
    for (let i = 0; i < pauseList.length; i++) {
        pauseList.item(i).classList.remove("controllightgray");
        pauseList.item(i).classList.remove("controllightturquoise");
        pauseList.item(i).classList.remove("controllightbrown");
        pauseList.item(i).classList.remove("controldarkgray");
        pauseList.item(i).classList.remove("controldarkturquoise");
        pauseList.item(i).classList.remove("controldarkbrown");
        pauseList.item(i).classList.add("controlprivate");
    }

    let resumeList = document.getElementsByClassName("resumeButton");
    for (let i = 0; i < resumeList.length; i++) {
        resumeList.item(i).classList.remove("controllightgray");
        resumeList.item(i).classList.remove("controllightturquoise");
        resumeList.item(i).classList.remove("controllightbrown");
        resumeList.item(i).classList.remove("controldarkgray");
        resumeList.item(i).classList.remove("controldarkturquoise");
        resumeList.item(i).classList.remove("controldarkbrown");
        resumeList.item(i).classList.add("controlprivate");
    }

    let stopList = document.getElementsByClassName("stopButton");
    for (let i = 0; i < stopList.length; i++) {
        stopList.item(i).classList.remove("controllightgray");
        stopList.item(i).classList.remove("controllightturquoise");
        stopList.item(i).classList.remove("controllightbrown");
        stopList.item(i).classList.remove("controldarkgray");
        stopList.item(i).classList.remove("controldarkturquoise");
        stopList.item(i).classList.remove("controldarkbrown");
        stopList.item(i).classList.add("controlprivate");
    }

    let titleList = document.getElementsByClassName("itemTitle");
    for (let i = 0; i < titleList.length; i++) {
        titleList.item(i).style.color = "white";
    }

    let sizeList = document.getElementsByClassName("itemSize");
    for (let i = 0; i < sizeList.length; i++) {
        sizeList.item(i).style.color = "white";
    }

    let rateList = document.getElementsByClassName("itemTransferRate");
    for (let i = 0; i < rateList.length; i++) {
        rateList.item(i).style.color = "white";
    }

    document.body.style.backgroundColor = "#3c3244";
    document.getElementById("pageToolBar").style.backgroundColor = "#3c3244";
    document.getElementById("pageBody").style.backgroundColor = "#3c3244";
    document.getElementById("eViewMessage").style.color = "white";
    document.getElementById("emptyView").style.color = "white";
    document.getElementById("pageTitle").style.color = "white";
    document.getElementById("downloadsManagerButton").classList.remove("lightturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("lightbrown");
    document.getElementById("downloadsManagerButton").classList.remove("darkgray");
    document.getElementById("downloadsManagerButton").classList.remove("darkturquoise");
    document.getElementById("downloadsManagerButton").classList.remove("darkbrown");
    document.getElementById("downloadsManagerButton").classList.remove("lightgray");
    document.getElementById("downloadsManagerButton").classList.add("private");
}