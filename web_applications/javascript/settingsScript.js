"use strict";
let selectedOption = "Ololan account";
let settingsCore = null;
let editingSearchEngine = null;
let updateAnimationIndex = 0;
let animationArrayIndex = [];

function setupConnector() {
    new QWebChannel(qt.webChannelTransport, function(ololanChannel) {
        settingsCore = ololanChannel.objects.settingsManager;
    });
}

function deleteAccount() {
    settingsCore.deleteAccount();
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

function setFocusInStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../images/search.png";
}

function setFocusOutStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    fieldIcon.src = "../../images/find.png";
}

function selectOption(clickEvent) {
    let folderButton = clickEvent.target;
    let folderClassName = folderButton.className.trim();
    let folderName = "";

    if ((folderClassName.localeCompare("buttonIcon") == 0) || (folderClassName.localeCompare("buttonText") == 0)) {
        folderName = folderButton.parentElement.textContent.trim();
    } else if (folderClassName.localeCompare("optionButton") == 0) {
        folderName = folderButton.textContent.trim();
    }

    if (folderName.localeCompare(selectedOption) != 0) {
        selectedOption = folderName;
        setOptionStyle(folderName);
    }
}

function selectOptionView(optionIndex) {
    let optionView = document.getElementsByClassName("optionContainer");
    for (let i = 0; i < optionView.length; i++) {
        if (i == optionIndex) {
            optionView.item(optionIndex).style.display = "block";
        } else {
            optionView.item(i).style.display = "none";
        }
    }
}

function setOptionStyle(folderName) {
    let folderArray = document.getElementsByClassName("optionButton");
    let iname = document.getElementsByClassName("buttonText");

    for (let i = 0; i < folderArray.length; i++) {
        let fName = iname.item(i).textContent.trim();
        if (folderName.localeCompare(fName) == 0) {
            folderArray.item(i).style.cssText = "background-color: #f9f9fb; border-top: 1px solid #f9f9fb; border-bottom: 1px solid #f9f9fb; border-right: 1px solid #f9f9fb; border-left: 7px solid #56bcc5; padding-left: 4px;";
            folderArray.item(i).onmouseover = function() { folderArray.item(i).style.cssText = "background-color: #f9f9fb; border-top: 1px solid #f9f9fb; border-bottom: 1px solid #f9f9fb; border-right: 1px solid #f9f9fb; border-left: 7px solid #56bcc5; padding-left: 4px;"; }
            folderArray.item(i).onmouseleave = function() { folderArray.item(i).style.cssText = "background-color: #f9f9fb; border-top: 1px solid #f9f9fb; border-bottom: 1px solid #f9f9fb; border-right: 1px solid #f9f9fb; border-left: 7px solid #56bcc5; padding-left: 4px;"; }
            selectOptionView(i);
        } else {
            folderArray.item(i).style.cssText = "background-color: transparent; border: 1px solid #ffffff; padding-left: 10px;";
            folderArray.item(i).onmouseover = function() { folderArray.item(i).style.cssText = "background-color: #f9f9fb; border: 1px solid #f9f9fb;"; }
            folderArray.item(i).onmouseleave = function() { folderArray.item(i).style.cssText = "background-color: transparent; border: 1px solid #ffffff;"; }
        }
    }
}

function showSavedCredentials(clickEvent) {
    let buttonClicked = clickEvent.target;
    let optionRowContent = document.getElementsByClassName("optionRowContent");

    if ((optionRowContent.item(2).style.display.localeCompare("none") == 0) || (optionRowContent.item(2).style.display.length < 1)) {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Hide";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Hide";
        }

        optionRowContent.item(2).style.display = "block";
        optionRowContent.item(2).classList.remove("hide");
    } else {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Show";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Show";
        }

        optionRowContent.item(2).classList.add("hide");
        setTimeout(function() { optionRowContent.item(2).style.display = "none"; }, 140);
    }
}

function showPaymentMethod(clickEvent) {
    let buttonClicked = clickEvent.target;
    let optionRowContent = document.getElementsByClassName("optionRowContent");

    if ((optionRowContent.item(3).style.display.localeCompare("none") == 0) || (optionRowContent.item(3).style.display.length < 1)) {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Hide";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Hide";
        }

        optionRowContent.item(3).style.display = "block";
        optionRowContent.item(3).classList.remove("hide");
    } else {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Show";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Show";
        }

        optionRowContent.item(3).classList.add("hide");
        setTimeout(function() { optionRowContent.item(3).style.display = "none"; }, 140);
    }
}

function showCustomFonts(clickEvent) {
    let buttonClicked = clickEvent.target;
    let optionRowContent = document.getElementsByClassName("optionRowContent");

    if ((optionRowContent.item(0).style.display.localeCompare("none") == 0) || (optionRowContent.item(0).style.display.length < 1)) {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Hide custom fonts";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Hide custom fonts";
        }

        optionRowContent.item(0).style.display = "block";
        optionRowContent.item(0).classList.remove("hide");
    } else {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Show custom fonts";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Show custom fonts";
        }

        optionRowContent.item(0).classList.add("hide");
        setTimeout(function() { optionRowContent.item(0).style.display = "none"; }, 140);
    }
}

function openSpecificPages(state) {
    let optionRowContent = document.getElementsByClassName("optionRowContent");
    let specificPage = document.getElementsByClassName("specificPageOption");

    if (state == true) {
        specificPage.item(0).style.display = "flex";
        specificPage.item(0).classList.remove("hide");
        optionRowContent.item(1).style.display = "block";
        optionRowContent.item(1).classList.remove("hide");
    } else {
        specificPage.item(0).classList.add("hide");
        optionRowContent.item(1).classList.add("hide");
        setTimeout(function() {
            specificPage.item(0).style.display = "none";
            optionRowContent.item(1).style.display = "none";
        }, 140);
    }
}

function setDefaultRangeValue() {
    let rangeInput = document.getElementsByClassName("rangeInput");
    let rangeValue = document.getElementById("defaultRangeValue");
    rangeValue.textContent = rangeInput.item(0).value;
    settingsCore.setCustomFontSize(rangeInput.item(0).value);
}

function setCustomFontSizeValue(value) {
    let rangeInput = document.getElementsByClassName("rangeInput");
    rangeInput.item(0).value = value;
    let rangeValue = document.getElementById("defaultRangeValue");
    rangeValue.textContent = value;
}

function setMinimumRangeValue() {
    let rangeInput = document.getElementsByClassName("rangeInput");
    let rangeValue = document.getElementById("minimumRangeValue");
    rangeValue.textContent = rangeInput.item(1).value;
    settingsCore.setMinimumFontSize(rangeInput.item(1).value);
}

function setMinimumFontSizeValue(value) {
    let rangeInput = document.getElementsByClassName("rangeInput");
    rangeInput.item(1).value = value;
    let rangeValue = document.getElementById("minimumRangeValue");
    rangeValue.textContent = value;
}

function showBrowingDataMenu(clickEvent) {
    let buttonClicked = clickEvent.target;
    let optionRowContent = document.getElementsByClassName("optionRowContent");

    if ((optionRowContent.item(3).style.display.localeCompare("none") == 0) || (optionRowContent.item(3).style.display.length < 1)) {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Hide";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Hide";
        }

        document.getElementById("clbButton").style.display = "flex";
        optionRowContent.item(3).style.display = "block";
        optionRowContent.item(3).classList.remove("hide");
    } else {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Manage data";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Manage data";
        }

        document.getElementById("clbButton").style.display = "none";
        optionRowContent.item(3).classList.add("hide");
        setTimeout(function() { optionRowContent.item(3).style.display = "none"; }, 140);
    }
}

function showCookiesMenu(clickEvent) {
    let buttonClicked = clickEvent.target;
    let optionRowContent = document.getElementsByClassName("optionRowContent");

    if ((optionRowContent.item(4).style.display.localeCompare("none") == 0) || (optionRowContent.item(4).style.display.length < 1)) {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Hide";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Hide";
        }
        //dont forget to init (optionRowContent.item(5).innerHTML = "") on loading settingView
        if (optionRowContent.item(4).childNodes.length > 1) {
            document.getElementById("clcButton").style.display = "flex";
        }

        optionRowContent.item(4).style.display = "block";
        optionRowContent.item(4).classList.remove("hide");
    } else {
        if (buttonClicked.className.trim().localeCompare("rowButton") == 0) {
            buttonClicked.childNodes.item(1).textContent = "Manage cookies";
        } else if (buttonClicked.className.trim().localeCompare("rButtonText") == 0) {
            buttonClicked.textContent = "Manage cookies";
        }

        document.getElementById("clcButton").style.display = "none";
        optionRowContent.item(4).classList.add("hide");
        setTimeout(function() { optionRowContent.item(4).style.display = "none"; }, 140);
    }
}

function synchronize() {
    let syncButton = document.getElementById("syncButton");
    if (syncButton.dataset.syncState.localeCompare("synchronized") == 0) {
        settingsCore.synchronize("Sign out");
    } else {
        settingsCore.synchronize("Sign in");
    }
}

function setSyncState(state, username) {
    let syncButton = document.getElementById("syncButton");
    let syncInfo = document.getElementsByClassName("accountStatus").item(0);
    let syncShortID = document.getElementsByClassName("accountName").item(0);

    if (state.localeCompare("Signed in") == 0) {
        document.getElementById("accountDeletionSpace").style.display = "flex";
        syncButton.textContent = "Log out";
        syncShortID.textContent = username.split(" ")[0].charAt(0).toUpperCase() + username.split(" ")[1].charAt(0).toUpperCase();
        syncInfo.textContent = username + " - Logged in";
        syncButton.dataset.syncState = "synchronized";
    } else if (state.localeCompare("Signed out") == 0) {
        document.getElementById("accountDeletionSpace").style.display = "none";
        syncButton.textContent = "Sign in";
        syncShortID.textContent = "GU";
        syncInfo.textContent = "Guest User - Not signed in";
        syncButton.dataset.syncState = "not synchronized";
    }
}

function sendUsageData() {
    let usageData = document.getElementById("usageData");
    if (usageData.checked == true) {
        settingsCore.sendUsageData("true");
    } else {
        settingsCore.sendUsageData("false");
    }
}

function setSendUsageData(state) {
    let usageData = document.getElementById("usageData");
    usageData.checked = state;
}

function selectTheme() {
    let themeColorList = document.getElementsByClassName("colorTheme");
    let colorList = ["Light gray", "Light turquoise", "Light brown", "Dark gray", "Dark turquoise", "Dark brown"];

    for (let i = 0; i < themeColorList.length; i++) {
        if (themeColorList.item(i).checked == true) {
            settingsCore.selectTheme(colorList[i]);
            break;
        }
    }
}

function setSelectedThemeColor(themeColor) {
    let themeColorList = document.getElementsByClassName("colorTheme");
    let colorList = ["Light gray", "Light turquoise", "Light brown", "Dark gray", "Dark turquoise", "Dark brown"];

    for (let i = 0; i < colorList.length; i++) {
        if (colorList[i].localeCompare(themeColor) == 0) {
            themeColorList.item(i).checked = true;
            break;
        }
    }
}

function showBookmarkBar() {
    let bookmarkBar = document.getElementById("bookmarkBar");
    if (bookmarkBar.checked == true) {
        settingsCore.showBookmarkBar("true");
    } else {
        settingsCore.showBookmarkBar("false");
    }
}

function setBookmarkBarState(state) {
    let bookmarkBar = document.getElementById("bookmarkBar");
    bookmarkBar.checked = state;
}

function selectPageZoom() {
    let pageZoom = document.getElementById("pageZoom");
    settingsCore.selectPageZoom(pageZoom.value);
}

function setPageZoom(zoomValue) {
    let pageZoom = document.getElementById("pageZoom");
    if (zoomValue.localeCompare("100") == 0) {
        pageZoom.value = "100% (default zoom)";
    } else {
        pageZoom.value = (zoomValue + "%");
    }

}

function selectFontSize() {
    let pageFontSize = document.getElementById("pageFontSize");
    settingsCore.selectFontSize(pageFontSize.value);
    if (pageFontSize.value.localeCompare("Custom") != 0) {
        document.getElementById("customFontValue").style.display = "none";
    }
}

function setSelectedFontSize(fontSize) {
    let pageFontSize = document.getElementById("pageFontSize");
    if (fontSize.localeCompare("Custom") == 0) {
        document.getElementById("customFontValue").style.display = "block";
    } else {
        document.getElementById("customFontValue").style.display = "none";
    }
    pageFontSize.value = fontSize;
}

function selectStandardFont() {
    let standardFont = document.getElementById("standardFont");
    settingsCore.selectStandardFont(standardFont.value);
}

function setSelectedStandardFont(standardFontValue) {
    let standardFont = document.getElementById("standardFont");
    standardFont.value = standardFontValue;
}

function selectSerifFont() {
    let serifFont = document.getElementById("serifFont");
    settingsCore.selectSerifFont(serifFont.value);
}

function setSelectedSerifFont(serifFontValue) {
    let serifFont = document.getElementById("serifFont");
    serifFont.value = serifFontValue;
}

function selectSansSerifFont() {
    let sansSerifFont = document.getElementById("sansSerifFont");
    settingsCore.selectSansSerifFont(sansSerifFont.value);
}

function setSelectedSansSerifFont(sansSerifFontValue) {
    let sansSerifFont = document.getElementById("sansSerifFont");
    sansSerifFont.value = sansSerifFontValue;
}

function selectFixedFont() {
    let fixedFont = document.getElementById("fixedFont");
    settingsCore.selectFixedFont(fixedFont.value);
}

function setSelectedFixedFont(fixedFontValue) {
    let fixedFont = document.getElementById("fixedFont");
    fixedFont.value = fixedFontValue;
}

function selectStartPageOption() {
    let startPageOptionList = document.getElementsByClassName("startPageOption");
    let optionList = ["start page", "left page", "specific page"];

    for (let i = 0; i < startPageOptionList.length; i++) {
        if (startPageOptionList.item(i).checked == true) {
            if (optionList[i].localeCompare("specific page") == 0) {
                openSpecificPages(true);
            } else {
                openSpecificPages(false);
            }
            settingsCore.selectStartPageOption(optionList[i]);
            break;
        }
    }
}

function setSelectedStartPageOption(startOption) {
    let startPageOptionList = document.getElementsByClassName("startPageOption");
    let optionList = ["start page", "left page", "specific page"];

    for (let i = 0; i < optionList.length; i++) {
        if (optionList[i].localeCompare(startOption) == 0) {
            startPageOptionList.item(i).checked = true;
            if (optionList[i].localeCompare("specific page") == 0) {
                openSpecificPages(true);
            } else {
                openSpecificPages(false);
            }
            break;
        }
    }
}

function loadSpecificPage(index, title, url, save) {
    let pageTitle = document.createElement("INPUT");
    pageTitle.className = "pageTitle";
    pageTitle.value = title;
    pageTitle.title = title;
    pageTitle.readOnly = true;

    let pageTitleContainer = document.createElement("DIV");
    pageTitleContainer.className = "pageTitleContainer";
    pageTitleContainer.appendChild(pageTitle);

    let pageUrl = document.createElement("INPUT");
    pageUrl.className = "pageUrl";
    pageUrl.setAttribute("type", "url");
    pageUrl.value = url;
    pageUrl.title = url;
    pageUrl.readOnly = true;

    let pageUrlContainer = document.createElement("DIV");
    pageUrlContainer.className = "pageUrlContainer";
    pageUrlContainer.appendChild(pageUrl);

    let spButtonEdit = document.createElement("BUTTON");
    spButtonEdit.className = "spButtonEdit";
    spButtonEdit.addEventListener("click", editSpecificPage);

    let spButtonDelete = document.createElement("BUTTON");
    spButtonDelete.className = "spButtonDelete";
    spButtonDelete.addEventListener("click", deleteSpecificPage);

    let spButtonContainerA = document.createElement("DIV");
    spButtonContainerA.className = "spButtonContainerA";
    spButtonContainerA.appendChild(spButtonEdit);
    spButtonContainerA.appendChild(spButtonDelete);

    let spButtonSave = document.createElement("BUTTON");
    spButtonSave.className = "spButtonSave";
    spButtonSave.addEventListener("click", saveSpecificPageEditing);

    let spButtonCancel = document.createElement("BUTTON");
    spButtonCancel.className = "spButtonCancel";
    spButtonCancel.addEventListener("click", cancelSpecificPageEditing);

    let spButtonContainerB = document.createElement("DIV");
    spButtonContainerB.className = "spButtonContainerB";
    spButtonContainerB.appendChild(spButtonSave);
    spButtonContainerB.appendChild(spButtonCancel);

    let spButtonContainer = document.createElement("DIV");
    spButtonContainer.className = "spButtonContainer";
    spButtonContainer.appendChild(spButtonContainerA);
    spButtonContainer.appendChild(spButtonContainerB);

    let specificPageData = document.createElement("DIV");
    specificPageData.className = "specificPageData";
    specificPageData.appendChild(pageTitleContainer);
    specificPageData.appendChild(pageUrlContainer);
    specificPageData.appendChild(spButtonContainer);

    let specificPage = document.createElement("DIV");
    specificPage.className = "specificPage";
    specificPage.appendChild(specificPageData);
    specificPage.dataset.index = index;

    let specificPageContainer = document.createElement("DIV");
    specificPageContainer.className = "specificPageContainer";
    specificPageContainer.appendChild(specificPage);

    let containerList = document.getElementsByClassName("optionRowContent");
    containerList.item(1).appendChild(specificPageContainer);

    if (save) {
        spButtonEdit.click();
        settingsCore.addSpecificPage(title, url);
    }
}

function setLastAddedIndex(lastItemAdded) {
    let pages = document.getElementsByClassName("specificPage");
    pages.item(pages.length - 1).dataset.index = lastItemAdded;
}

function addSpecificPage() {
    loadSpecificPage(22, "New Tab", "ololan://home", true);
}

function editSpecificPage(event) {
    let button = event.target;
    let specificPageData = button.parentElement.parentElement.parentElement;

    specificPageData.childNodes.item(0).classList.add("editing");
    specificPageData.childNodes.item(0).childNodes.item(0).readOnly = false;
    specificPageData.childNodes.item(0).childNodes.item(0).focus();

    specificPageData.childNodes.item(1).classList.add("editing");
    specificPageData.childNodes.item(1).childNodes.item(0).readOnly = false;

    specificPageData.childNodes.item(2).childNodes.item(0).style.display = "none";
    specificPageData.childNodes.item(2).childNodes.item(1).style.display = "flex";
}

function deleteSpecificPage(event) {
    let button = event.target;
    let specificPage = button.parentElement.parentElement.parentElement.parentElement;
    let specificPageData = button.parentElement.parentElement.parentElement;
    let title = specificPageData.childNodes.item(0).childNodes.item(0).value;
    let url = specificPageData.childNodes.item(1).childNodes.item(0).value;
    let index = specificPage.dataset.index;
    settingsCore.deleteSpecificPage(title, url, index);
    specificPage.remove();
}

function saveSpecificPageEditing(event) {
    let button = event.target;
    let specificPageData = button.parentElement.parentElement.parentElement;
    let specificPage = button.parentElement.parentElement.parentElement.parentElement;
    let index = specificPage.dataset.index;

    let title = specificPageData.childNodes.item(0).childNodes.item(0).value;
    specificPageData.childNodes.item(0).childNodes.item(0).title = title;
    specificPageData.childNodes.item(0).childNodes.item(0).readOnly = true;
    specificPageData.childNodes.item(0).childNodes.item(0).blur();
    specificPageData.childNodes.item(0).classList.remove("editing");

    let url = specificPageData.childNodes.item(1).childNodes.item(0).value;
    specificPageData.childNodes.item(1).childNodes.item(0).title = url;
    specificPageData.childNodes.item(1).childNodes.item(0).readOnly = true;
    specificPageData.childNodes.item(1).childNodes.item(0).blur();
    specificPageData.childNodes.item(1).classList.remove("editing");

    specificPageData.childNodes.item(2).childNodes.item(0).style.display = "flex";
    specificPageData.childNodes.item(2).childNodes.item(1).style.display = "none";
    settingsCore.saveSpecificPageEditing(index, title, url);
}

function cancelSpecificPageEditing(event) {
    let button = event.target;
    let specificPageData = button.parentElement.parentElement.parentElement;

    specificPageData.childNodes.item(0).classList.remove("editing");
    specificPageData.childNodes.item(0).childNodes.item(0).value = specificPageData.childNodes.item(0).childNodes.item(0).title;
    specificPageData.childNodes.item(0).childNodes.item(0).blur();
    specificPageData.childNodes.item(0).childNodes.item(0).readOnly = true;

    specificPageData.childNodes.item(1).classList.remove("editing");
    specificPageData.childNodes.item(1).childNodes.item(0).value = specificPageData.childNodes.item(1).childNodes.item(0).title;
    specificPageData.childNodes.item(1).childNodes.item(0).blur();
    specificPageData.childNodes.item(1).childNodes.item(0).readOnly = true;

    specificPageData.childNodes.item(2).childNodes.item(0).style.display = "flex";
    specificPageData.childNodes.item(2).childNodes.item(1).style.display = "none";
}

function allowJavascript() {
    let enableJavascript = document.getElementById("enableJavascript");
    if (enableJavascript.checked == true) {
        settingsCore.allowJavascript("true");
    } else {
        settingsCore.allowJavascript("false");
    }
}

function setAllowJavascript(state) {
    let enableJavascript = document.getElementById("enableJavascript");
    enableJavascript.checked = state;
}

function allowJSCopyPaste() {
    let enableJSCopyPaste = document.getElementById("enableJSCopyPaste");
    if (enableJSCopyPaste.checked == true) {
        settingsCore.allowJSCopyPaste("true");
    } else {
        settingsCore.allowJSCopyPaste("false");
    }
}

function setAllowJSCopyPaste(state) {
    let enableJSCopyPaste = document.getElementById("enableJSCopyPaste");
    enableJSCopyPaste.checked = state;
}

function allowJSPopUps() {
    let enableJSPopUps = document.getElementById("enableJSPopUps");
    if (enableJSPopUps.checked == true) {
        settingsCore.allowJSPopUps("true");
    } else {
        settingsCore.allowJSPopUps("false");
    }
}

function setAllowJSPopUps(state) {
    let enableJSPopUps = document.getElementById("enableJSPopUps");
    enableJSPopUps.checked = state;
}

function enablePlugins() {
    let plugins = document.getElementById("plugins");
    if (plugins.checked == true) {
        settingsCore.enablePlugins("true");
    } else {
        settingsCore.enablePlugins("false");
    }
}

function setEnablePlugins(state) {
    let plugins = document.getElementById("plugins");
    plugins.checked = state;
}

function allowInsecureContents() {
    let insecureContents = document.getElementById("insecureContents");
    if (insecureContents.checked == true) {
        settingsCore.allowInsecureContents("true");
    } else {
        settingsCore.allowInsecureContents("false");
    }
}

function setAllowInsecureContents(state) {
    let insecureContents = document.getElementById("insecureContents");
    insecureContents.checked = state;
}

function enableInsecureGeolocation() {
    let insecureGeolocation = document.getElementById("insecureGeolocation");
    if (insecureGeolocation.checked == true) {
        settingsCore.enableInsecureGeolocation("true");
    } else {
        settingsCore.enableInsecureGeolocation("false");
    }
}

function setEnableInsecureGeolocation(state) {
    let insecureGeolocation = document.getElementById("insecureGeolocation");
    insecureGeolocation.checked = state;
}

function enableGeolocation() {
    let geolocation = document.getElementById("geolocation");
    if (geolocation.checked == true) {
        settingsCore.enableGeolocation("true");
    } else {
        settingsCore.enableGeolocation("false");
    }
}

function setEnableGeolocation(state) {
    let geolocation = document.getElementById("geolocation");
    geolocation.checked = state;
}

function enableDnsPrefetch() {
    let dnsPrefetch = document.getElementById("dnsPrefetch");
    if (dnsPrefetch.checked == true) {
        settingsCore.enableDnsPrefetch("true");
    } else {
        settingsCore.enableDnsPrefetch("false");
    }
}

function setEnableDnsPrefetch(state) {
    let dnsPrefetch = document.getElementById("dnsPrefetch");
    dnsPrefetch.checked = state;
}

function manageCertificates() {
    settingsCore.manageCertificates();
}

function enableInternalPDFViewer() {
    let pdfInternalView = document.getElementById("pdfInternalView");
    if (pdfInternalView.checked == true) {
        settingsCore.enableInternalPDFViewer("true");
    } else {
        settingsCore.enableInternalPDFViewer("false");
    }
}

function setEnableInternalPDFViewer(state) {
    let pdfInternalView = document.getElementById("pdfInternalView");
    pdfInternalView.checked = state;
}

function enableMicrophone() {
    let microphone = document.getElementById("microphone");
    if (microphone.checked == true) {
        settingsCore.enableMicrophone("true");
    } else {
        settingsCore.enableMicrophone("false");
    }
}

function setEnableMicrophone(state) {
    let microphone = document.getElementById("microphone");
    microphone.checked = state;
}

function enableCamera() {
    let camera = document.getElementById("camera");
    if (camera.checked == true) {
        settingsCore.enableCamera("true");
    } else {
        settingsCore.enableCamera("false");
    }
}

function setEnableCamera(state) {
    let camera = document.getElementById("camera");
    camera.checked = state;
}

function selectWebProtection() {
    let webProtectionList = document.getElementsByClassName("webProtection");
    let webProtection = ["strongProtection", "standardProtection", "noneProtection"];

    for (let i = 0; i < webProtectionList.length; i++) {
        if (webProtectionList.item(i).checked == true) {
            settingsCore.selectWebProtection(webProtection[i]);
            showWebProtectionDetails(i);
            break;
        }
    }
}

function showWebProtectionDetails(protectionIndex) {
    let protectionDetails = document.getElementsByClassName("optionRowDetails");
    if (protectionIndex == 0) {
        protectionDetails.item(0).style.display = "flex";
        setTimeout(function() { protectionDetails.item(0).classList.add("showOptionDetails"); }, 1);
        protectionDetails.item(1).style.display = "none";
        protectionDetails.item(1).classList.remove("showOptionDetails");
        protectionDetails.item(2).style.display = "none";
        protectionDetails.item(2).classList.remove("showOptionDetails");
    } else if (protectionIndex == 1) {
        protectionDetails.item(0).style.display = "none";
        protectionDetails.item(0).classList.remove("showOptionDetails");
        protectionDetails.item(1).style.display = "flex";
        setTimeout(function() { protectionDetails.item(1).classList.add("showOptionDetails"); }, 1);
        protectionDetails.item(2).style.display = "none";
        protectionDetails.item(2).classList.remove("showOptionDetails");
    } else if (protectionIndex == 2) {
        protectionDetails.item(0).style.display = "none";
        protectionDetails.item(0).classList.remove("showOptionDetails");
        protectionDetails.item(1).style.display = "none";
        protectionDetails.item(1).classList.remove("showOptionDetails");
        protectionDetails.item(2).style.display = "flex";
        setTimeout(function() { protectionDetails.item(2).classList.add("showOptionDetails"); }, 1);
    }
}

function setSelectedWebProtection(protection) {
    let webProtectionList = document.getElementsByClassName("webProtection");
    let webProtection = ["strongProtection", "standardProtection", "noneProtection"];

    for (let i = 0; i < webProtection.length; i++) {
        if (webProtection[i].localeCompare(protection) == 0) {
            webProtectionList.item(i).checked = true;
            showWebProtectionDetails(i);
            break;
        }
    }
}

function askToSavePassword() {
    let savePassword = document.getElementById("savePassword");
    if (savePassword.checked == true) {
        settingsCore.askToSavePassword("true");
    } else {
        settingsCore.askToSavePassword("false");
    }
}

function setAskToSavePassword(state) {
    let savePassword = document.getElementById("savePassword");
    savePassword.checked = state;
}

function websiteAutoSignIn() {
    let autoSignToWebsites = document.getElementById("autoSignToWebsites");
    if (autoSignToWebsites.checked == true) {
        settingsCore.websiteAutoSignIn("true");
    } else {
        settingsCore.websiteAutoSignIn("false");
    }
}

function setWebsiteAutoSignIn(state) {
    let autoSignToWebsites = document.getElementById("autoSignToWebsites");
    autoSignToWebsites.checked = state;
}

function loadCredentials(icon, webAddress, loginAddress, password) {
    let siteIcon = document.createElement("IMG");
    siteIcon.className = "siteIcon";
    siteIcon.src = icon;

    let cAddress = document.createElement("DIV");
    cAddress.className = "cAddress";
    cAddress.textContent = webAddress;

    let website = document.createElement("DIV");
    website.className = "website";
    website.appendChild(siteIcon);
    website.appendChild(cAddress);

    let cLogin = document.createElement("DIV");
    cLogin.className = "cLogin";
    cLogin.textContent = loginAddress;

    let cPassword = document.createElement("INPUT");
    cPassword.className = "cPassword";
    cPassword.setAttribute("type", "password");
    cPassword.value = password;
    cPassword.dataset.password = password;
    cPassword.readOnly = true;

    let passwordContainer = document.createElement("DIV");
    passwordContainer.className = "passwordContainer";
    passwordContainer.appendChild(cPassword);

    let cButtonView = document.createElement("BUTTON");
    cButtonView.className = "cButtonView";
    cButtonView.addEventListener("click", revealCredentialPassword);

    let cButtonEdit = document.createElement("BUTTON");
    cButtonEdit.className = "cButtonEdit";
    cButtonEdit.addEventListener("click", editCredentialPassword);

    let cButtonDelete = document.createElement("BUTTON");
    cButtonDelete.className = "cButtonDelete";
    cButtonDelete.addEventListener("click", deleteCredential);

    let cButtonContainerA = document.createElement("DIV");
    cButtonContainerA.className = "cButtonContainerA";
    cButtonContainerA.appendChild(cButtonView);
    cButtonContainerA.appendChild(cButtonEdit);
    cButtonContainerA.appendChild(cButtonDelete);

    let cButtonViewB = document.createElement("BUTTON");
    cButtonViewB.className = "cButtonView";
    cButtonViewB.addEventListener("click", revealCredentialPassword);

    let cButtonSave = document.createElement("BUTTON");
    cButtonSave.className = "cButtonSave";
    cButtonSave.addEventListener("click", saveCredentialPassword);

    let cButtonCancel = document.createElement("BUTTON");
    cButtonCancel.className = "cButtonCancel";
    cButtonCancel.addEventListener("click", cancelCredentialEditing);

    let cButtonContainerB = document.createElement("DIV");
    cButtonContainerB.className = "cButtonContainerB";
    cButtonContainerB.appendChild(cButtonViewB);
    cButtonContainerB.appendChild(cButtonSave);
    cButtonContainerB.appendChild(cButtonCancel);

    let cButtonContainer = document.createElement("DIV");
    cButtonContainer.className = "cButtonContainer";
    cButtonContainer.appendChild(cButtonContainerA);
    cButtonContainer.appendChild(cButtonContainerB);

    let signInData = document.createElement("DIV");
    signInData.className = "signInData";
    signInData.appendChild(cLogin);
    signInData.appendChild(passwordContainer);
    signInData.appendChild(cButtonContainer);

    let credential = document.createElement("DIV");
    credential.className = "credential";
    credential.appendChild(signInData);

    let credentialContainer = document.createElement("DIV");
    credentialContainer.className = "credentialContainer";
    credentialContainer.appendChild(website);
    credentialContainer.appendChild(credential);

    let containerList = document.getElementsByClassName("optionRowContent");
    containerList.item(2).appendChild(credentialContainer);
}

function editCredentialPassword(event) {
    let button = event.target;
    let signInData = button.parentElement.parentElement.parentElement;

    signInData.childNodes.item(2).childNodes.item(0).childNodes.item(0).style.backgroundImage = "url(../images/hidePwd.png)";
    signInData.childNodes.item(2).childNodes.item(1).childNodes.item(0).style.backgroundImage = "url(../images/hidePwd.png)";
    signInData.childNodes.item(1).childNodes.item(0).setAttribute("type", "text");
    signInData.childNodes.item(1).classList.add("editing");
    signInData.childNodes.item(1).childNodes.item(0).readOnly = false;
    signInData.childNodes.item(1).childNodes.item(0).focus();

    signInData.childNodes.item(2).childNodes.item(0).style.display = "none";
    signInData.childNodes.item(2).childNodes.item(1).style.display = "flex";
}

function deleteCredential(event) {
    let button = event.target;
    let credential = button.parentElement.parentElement.parentElement.parentElement.parentElement;
    let index = credential.dataset.index;
    settingsCore.deleteCredential(index);
    credential.remove();
}

function saveCredentialPassword(event) {
    let button = event.target;
    let signInData = button.parentElement.parentElement.parentElement;
    let credential = button.parentElement.parentElement.parentElement.parentElement.parentElement;
    let index = credential.dataset.index;

    let password = signInData.childNodes.item(1).childNodes.item(0).value;
    signInData.childNodes.item(1).childNodes.item(0).dataset.password = password;
    signInData.childNodes.item(1).childNodes.item(0).readOnly = true;
    signInData.childNodes.item(1).childNodes.item(0).blur();
    signInData.childNodes.item(1).classList.remove("editing");
    signInData.childNodes.item(1).childNodes.item(0).setAttribute("type", "password");
    signInData.childNodes.item(2).childNodes.item(0).childNodes.item(0).style.backgroundImage = "url(../images/revealPwd.png)";
    signInData.childNodes.item(2).childNodes.item(1).childNodes.item(0).style.backgroundImage = "url(../images/revealPwd.png)";

    signInData.childNodes.item(2).childNodes.item(0).style.display = "flex";
    signInData.childNodes.item(2).childNodes.item(1).style.display = "none";
    settingsCore.saveCredentialPassword(index, password);
}

function cancelCredentialEditing(event) {
    let button = event.target;
    let signInData = button.parentElement.parentElement.parentElement;

    signInData.childNodes.item(1).classList.remove("editing");
    signInData.childNodes.item(1).childNodes.item(0).value = signInData.childNodes.item(1).childNodes.item(0).dataset.password;
    signInData.childNodes.item(1).childNodes.item(0).blur();
    signInData.childNodes.item(1).childNodes.item(0).readOnly = true;
    signInData.childNodes.item(1).childNodes.item(0).setAttribute("type", "password");
    signInData.childNodes.item(2).childNodes.item(0).childNodes.item(0).style.backgroundImage = "url(../images/revealPwd.png)";
    signInData.childNodes.item(2).childNodes.item(1).childNodes.item(0).style.backgroundImage = "url(../images/revealPwd.png)";

    signInData.childNodes.item(2).childNodes.item(0).style.display = "flex";
    signInData.childNodes.item(2).childNodes.item(1).style.display = "none";
}

function revealCredentialPassword(event) {
    let button = event.target;
    let signInData = button.parentElement.parentElement.parentElement;

    if (signInData.childNodes.item(1).childNodes.item(0).getAttribute("type").localeCompare("password") == 0) {
        signInData.childNodes.item(1).childNodes.item(0).setAttribute("type", "text");
        signInData.childNodes.item(2).childNodes.item(0).childNodes.item(0).style.backgroundImage = "url(../images/hidePwd.png)";
        signInData.childNodes.item(2).childNodes.item(1).childNodes.item(0).style.backgroundImage = "url(../images/hidePwd.png)";
    } else {
        signInData.childNodes.item(1).childNodes.item(0).setAttribute("type", "password");
        signInData.childNodes.item(2).childNodes.item(0).childNodes.item(0).style.backgroundImage = "url(../images/revealPwd.png)";
        signInData.childNodes.item(2).childNodes.item(1).childNodes.item(0).style.backgroundImage = "url(../images/revealPwd.png)";
    }
}

function autoSavePaymentMethods() {
    let paymentMethods = document.getElementById("paymentMethods");
    if (paymentMethods.checked == true) {
        settingsCore.autoSavePaymentMethods("true");
    } else {
        settingsCore.autoSavePaymentMethods("false");
    }
}

function setAutoSavePaymentMethods(state) {
    let paymentMethods = document.getElementById("paymentMethods");
    paymentMethods.checked = state;
}

function loadPaymentMethods(name, number, expiration) {
    let cardName = document.createElement("INPUT");
    cardName.className = "cardName";
    cardName.value = name;
    cardName.title = name;
    cardName.readOnly = true;

    let cardContainer = document.createElement("DIV");
    cardContainer.className = "cardContainer";
    cardContainer.appendChild(cardName);

    let size = number.length;
    let code = number.charAt(size - 4) + number.charAt(size - 3) + number.charAt(size - 2) + number.charAt(size - 1);

    let cardNumber = document.createElement("INPUT");
    cardNumber.className = "cardNumber";
    cardNumber.value = "****" + code;
    cardNumber.title = "****" + code;
    cardNumber.dataset.number = number;
    cardNumber.readOnly = true;

    let cardContainerB = document.createElement("DIV");
    cardContainerB.className = "cardContainer";
    cardContainerB.appendChild(cardNumber);

    let monthSelection = document.createElement("SELECT");
    monthSelection.className = "monthSelection";
    monthSelection.setAttribute("name", "monthSelection");

    for (let i = 1; i < 13; i++) {
        let month = document.createElement("OPTION");
        if (i < 10) {
            month.textContent = "0" + i;
        } else {
            month.textContent = i;
        }
        monthSelection.appendChild(month);
    }

    let monthContainer = document.createElement("DIV");
    monthContainer.className = "monthContainer";
    monthContainer.appendChild(monthSelection);

    let yearSelection = document.createElement("SELECT");
    yearSelection.className = "yearSelection";
    yearSelection.setAttribute("name", "yearSelection");

    for (let i = 2022; i < 2039; i++) {
        let year = document.createElement("OPTION");
        year.textContent = i;
        yearSelection.appendChild(year);
    }

    let dateValue = expiration.split("/");
    monthSelection.value = dateValue[0];
    monthSelection.title = dateValue[0];
    yearSelection.title = dateValue[1];
    yearSelection.value = dateValue[1];

    let yearContainer = document.createElement("DIV");
    yearContainer.className = "yearContainer";
    yearContainer.appendChild(yearSelection);

    let datePanel = document.createElement("DIV");
    datePanel.className = "datePanel";
    datePanel.appendChild(monthContainer);
    datePanel.appendChild(yearContainer);

    let cardDate = document.createElement("DIV");
    cardDate.className = "cardDate";
    cardDate.textContent = "cardExpiration";

    let cardExpiration = document.createElement("DIV");
    cardExpiration.className = "cardExpiration";
    cardExpiration.appendChild(datePanel);
    cardExpiration.appendChild(cardDate);

    let pButtonEdit = document.createElement("BUTTON");
    pButtonEdit.className = "pButtonEdit";
    pButtonEdit.addEventListener("click", editPaymentMethod);

    let pButtonDelete = document.createElement("BUTTON");
    pButtonDelete.className = "pButtonDelete";
    pButtonDelete.addEventListener("click", deletePaymentMethod);

    let pButtonContainerA = document.createElement("DIV");
    pButtonContainerA.className = "pButtonContainerA";
    pButtonContainerA.appendChild(pButtonEdit);
    pButtonContainerA.appendChild(pButtonDelete);

    let pButtonSave = document.createElement("BUTTON");
    pButtonSave.className = "pButtonSave";
    pButtonSave.addEventListener("click", savePaymentEditing);

    let pButtonCancel = document.createElement("BUTTON");
    pButtonCancel.className = "pButtonCancel";
    pButtonCancel.addEventListener("click", cancelPaymentEditing);

    let pButtonContainerB = document.createElement("DIV");
    pButtonContainerB.className = "pButtonContainerB";
    pButtonContainerB.appendChild(pButtonSave);
    pButtonContainerB.appendChild(pButtonCancel);

    let pButtonContainer = document.createElement("DIV");
    pButtonContainer.className = "pButtonContainer";
    pButtonContainer.appendChild(pButtonContainerA);
    pButtonContainer.appendChild(pButtonContainerB);

    let paymentData = document.createElement("DIV");
    paymentData.className = "paymentData";
    paymentData.appendChild(cardContainer);
    paymentData.appendChild(cardContainerB);
    paymentData.appendChild(cardExpiration);
    paymentData.appendChild(pButtonContainer);

    let paymentMethod = document.createElement("DIV");
    paymentMethod.className = "paymentMethod";
    paymentMethod.appendChild(paymentData);

    let paymentContainer = document.createElement("DIV");
    paymentContainer.className = "paymentContainer";
    paymentContainer.appendChild(paymentMethod);

    let containerList = document.getElementsByClassName("optionRowContent");
    containerList.item(3).appendChild(paymentContainer);
}

function editPaymentMethod(event) {
    let button = event.target;
    let paymentData = button.parentElement.parentElement.parentElement;

    paymentData.childNodes.item(0).classList.add("editing");
    paymentData.childNodes.item(0).childNodes.item(0).readOnly = false;
    paymentData.childNodes.item(0).childNodes.item(0).focus();

    paymentData.childNodes.item(1).classList.add("editing");
    paymentData.childNodes.item(1).childNodes.item(0).readOnly = false;
    paymentData.childNodes.item(1).childNodes.item(0).value = paymentData.childNodes.item(1).childNodes.item(0).dataset.number;

    paymentData.childNodes.item(2).childNodes.item(1).style.display = "none";
    paymentData.childNodes.item(2).childNodes.item(0).style.display = "flex";

    let month = paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(0).childNodes.item(0).title;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(0).childNodes.item(0).value = month;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(0).classList.add("editing");

    let year = paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(1).childNodes.item(0).title;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(1).childNodes.item(0).value = year;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(1).classList.add("editing");

    paymentData.childNodes.item(3).childNodes.item(0).style.display = "none";
    paymentData.childNodes.item(3).childNodes.item(1).style.display = "flex";
}

function deletePaymentMethod(event) {
    let button = event.target;
    let paymentMethod = button.parentElement.parentElement.parentElement.parentElement.parentElement;
    let index = paymentMethod.dataset.index;
    settingsCore.deletePaymentMethod(index);
    paymentMethod.remove();
}

function savePaymentEditing(event) {
    let button = event.target;
    let paymentData = button.parentElement.parentElement.parentElement;
    let paymentMethod = button.parentElement.parentElement.parentElement.parentElement.parentElement;
    let index = paymentMethod.dataset.index;

    let cardName = paymentData.childNodes.item(0).childNodes.item(0).value;
    paymentData.childNodes.item(0).childNodes.item(0).title = cardName;
    paymentData.childNodes.item(0).childNodes.item(0).readOnly = true;
    paymentData.childNodes.item(0).childNodes.item(0).blur();
    paymentData.childNodes.item(0).classList.remove("editing");

    let cardNumber = paymentData.childNodes.item(1).childNodes.item(0).value;
    paymentData.childNodes.item(1).childNodes.item(0).dataset.number = cardNumber;
    paymentData.childNodes.item(1).childNodes.item(0).readOnly = true;
    paymentData.childNodes.item(1).childNodes.item(0).blur();
    paymentData.childNodes.item(1).classList.remove("editing");

    let month = paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(0).childNodes.item(0).value;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(0).childNodes.item(0).title = month;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(0).classList.remove("editing");

    let year = paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(1).childNodes.item(0).value;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(1).childNodes.item(0).title = year;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(1).classList.remove("editing");

    paymentData.childNodes.item(2).childNodes.item(0).style.display = "none";
    paymentData.childNodes.item(2).childNodes.item(1).style.display = "block";
    paymentData.childNodes.item(2).childNodes.item(1).textContent = "Expire " + month + "/" + year;

    paymentData.childNodes.item(3).childNodes.item(0).style.display = "flex";
    paymentData.childNodes.item(3).childNodes.item(1).style.display = "none";
    settingsCore.savePaymentEditing(index, cardName, cardNumber, month, year);
}

function cancelPaymentEditing(event) {
    let button = event.target;
    let paymentData = button.parentElement.parentElement.parentElement;

    paymentData.childNodes.item(0).classList.remove("editing");
    paymentData.childNodes.item(0).childNodes.item(0).value = paymentData.childNodes.item(0).childNodes.item(0).title;
    paymentData.childNodes.item(0).childNodes.item(0).blur();
    paymentData.childNodes.item(0).childNodes.item(0).readOnly = true;

    let month = paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(0).childNodes.item(0).title;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(0).classList.remove("editing");
    let year = paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(1).childNodes.item(0).title;
    paymentData.childNodes.item(2).childNodes.item(0).childNodes.item(1).classList.remove("editing");

    paymentData.childNodes.item(2).childNodes.item(0).style.display = "none";
    paymentData.childNodes.item(2).childNodes.item(1).style.display = "block";
    paymentData.childNodes.item(2).childNodes.item(1).textContent = "Expire " + month + "/" + year;

    paymentData.childNodes.item(1).classList.remove("editing");
    paymentData.childNodes.item(1).childNodes.item(0).value = paymentData.childNodes.item(1).childNodes.item(0).title;
    paymentData.childNodes.item(1).childNodes.item(0).blur();
    paymentData.childNodes.item(1).childNodes.item(0).readOnly = true;

    paymentData.childNodes.item(3).childNodes.item(0).style.display = "flex";
    paymentData.childNodes.item(3).childNodes.item(1).style.display = "none";
}

function selectCookiesPolicy() {
    let cookiePolicyList = document.getElementsByClassName("cookiePolicy");
    let cookiePolicy = ["enableCookies", "privateBrowsingCookies", "noneCookies"];

    for (let i = 0; i < cookiePolicyList.length; i++) {
        if (cookiePolicyList.item(i).checked == true) {
            settingsCore.selectCookiesPolicy(cookiePolicy[i]);
            showCookiesOptionDetails(i);
            break;
        }
    }
}

function showCookiesOptionDetails(cookiesOptionIndex) {
    let cookiesOptionDetails = document.getElementsByClassName("optionRowDetails");
    if (cookiesOptionIndex == 0) {
        cookiesOptionDetails.item(3).style.display = "flex";
        setTimeout(function() { cookiesOptionDetails.item(3).classList.add("showOptionDetails"); }, 1);
        cookiesOptionDetails.item(4).style.display = "none";
        cookiesOptionDetails.item(4).classList.remove("showOptionDetails");
        cookiesOptionDetails.item(5).style.display = "none";
        cookiesOptionDetails.item(5).classList.remove("showOptionDetails");
    } else if (cookiesOptionIndex == 1) {
        cookiesOptionDetails.item(3).style.display = "none";
        cookiesOptionDetails.item(3).classList.remove("showOptionDetails");
        cookiesOptionDetails.item(4).style.display = "flex";
        setTimeout(function() { cookiesOptionDetails.item(4).classList.add("showOptionDetails"); }, 1);
        cookiesOptionDetails.item(5).style.display = "none";
        cookiesOptionDetails.item(5).classList.remove("showOptionDetails");
    } else if (cookiesOptionIndex == 2) {
        cookiesOptionDetails.item(3).style.display = "none";
        cookiesOptionDetails.item(3).classList.remove("showOptionDetails");
        cookiesOptionDetails.item(4).style.display = "none";
        cookiesOptionDetails.item(4).classList.remove("showOptionDetails");
        cookiesOptionDetails.item(5).style.display = "flex";
        setTimeout(function() { cookiesOptionDetails.item(5).classList.add("showOptionDetails"); }, 1);
    }
}

function setSelectedCookiesPolicy(policy) {
    let cookiePolicyList = document.getElementsByClassName("cookiePolicy");
    let cookiePolicy = ["enableCookies", "privateBrowsingCookies", "noneCookies"];

    for (let i = 0; i < cookiePolicy.length; i++) {
        if (cookiePolicy[i].localeCompare(policy) == 0) {
            cookiePolicyList.item(i).checked = true;
            showCookiesOptionDetails(i);
            break;
        }
    }
}

function deleteCookiesOnClose() {
    let deleteCookies = document.getElementById("deleteCookies");
    if (deleteCookies.checked == true) {
        settingsCore.deleteCookiesOnClose("true");
    } else {
        settingsCore.deleteCookiesOnClose("false");
    }
}

function setDeleteCookiesOnClose(state) {
    let deleteCookies = document.getElementById("deleteCookies");
    deleteCookies.checked = state;
}

function enableDoNotTrack() {
    let doNotTrack = document.getElementById("doNotTrack");
    if (doNotTrack.checked == true) {
        settingsCore.enableDoNotTrack("true");
    } else {
        settingsCore.enableDoNotTrack("false");
    }
}

function setEnableDoNotTrack(state) {
    let doNotTrack = document.getElementById("doNotTrack");
    doNotTrack.checked = state;
}

function clearBrowsingData() {
    let valueA = "false";
    let valueB = "false";
    let valueC = "false";
    let valueD = "false";
    let valueE = "false";

    if (document.getElementById("cookieAndData").checked == true) {
        document.getElementById("cookieSize").textContent = "0 bytes";
        document.getElementById("cookieAndData").checked = false;
        valueA = "true";
    }
    if (document.getElementById("cachedData").checked == true) {
        document.getElementById("cacheSize").textContent = "0 bytes";
        document.getElementById("cachedData").checked = false;
        valueB = "true";
    }
    if (document.getElementById("browsingHistory").checked == true) {
        document.getElementById("browsingHistorySize").textContent = "0 bytes";
        document.getElementById("browsingHistory").checked = false;
        valueC = "true";
    }
    if (document.getElementById("passwordData").checked == true) {
        document.getElementById("passwordDataSize").textContent = "0 bytes";
        document.getElementById("passwordData").checked = false;
        valueD = "true";
    }
    /*if (document.getElementById("paymentData").checked == true) {
        document.getElementById("paymentDataSize").textContent = "0 bits";
        document.getElementById("paymentData").checked = false;
        valueE = "true";
    }*/
    settingsCore.clearBrowsingData(valueA, valueB, valueC, valueD, valueE);
}

function setBrowsingDataSize(cookieSize, cacheSize, browsingHistorySize, passwordDataSize, paymentDataSize) {
    document.getElementById("cookieSize").textContent = cookieSize;
    document.getElementById("cacheSize").textContent = cacheSize;
    document.getElementById("browsingHistorySize").textContent = browsingHistorySize;
    document.getElementById("passwordDataSize").textContent = passwordDataSize;
    //document.getElementById("paymentDataSize").textContent = paymentDataSize;
}

function loadCookieData(cookieTitle, cookieValue, cookieCount) {
    let itemTitle = document.createElement("DIV");
    itemTitle.className = "rowDetail";
    itemTitle.textContent = cookieTitle;

    let itemCount = document.createElement("DIV");
    itemCount.className = "rowDetail";
    itemCount.textContent = cookieCount + " cookie(s)";

    let coButtonDelete = document.createElement("BUTTON");
    coButtonDelete.className = "coButtonDelete";
    coButtonDelete.addEventListener("click", deleteCookie);

    let cookieItem = document.createElement("DIV");
    cookieItem.className = "optionRow";
    cookieItem.appendChild(itemTitle);
    cookieItem.appendChild(itemCount);
    cookieItem.appendChild(coButtonDelete);
    cookieItem.dataset.cookieName = cookieTitle;
    cookieItem.dataset.cookieValue = cookieValue;

    let containerList = document.getElementsByClassName("optionRowContent");
    containerList.item(5).appendChild(cookieItem);

    if ((document.getElementById("clcButton").style.display.localeCompare("none") == 0) || (document.getElementById("clcButton").style.display.length < 1)) {
        document.getElementById("clcButton").style.display = "flex";
    }
}

function deleteCookie(event) {
    let button = event.target;
    let cookieItem = button.parentElement;
    settingsCore.deleteCookie(cookieItem.dataset.cookieValue, cookieItem.dataset.cookieName);
    cookieItem.remove();

    let containerList = document.getElementsByClassName("optionRowContent");
    if (containerList.item(5).childNodes.length < 1) {
        document.getElementById("clcButton").style.display = "none";
    }
}

function clearAllCookies() {
    let containerList = document.getElementsByClassName("optionRowContent");
    containerList.item(5).innerHTML = "";
    document.getElementById("clcButton").style.display = "none";
    settingsCore.clearAllCookies();
}

function updateCookiesList() {
    let containerList = document.getElementsByClassName("optionRowContent");
    containerList.item(5).innerHTML = "";
}

function selectDefaultSearchEngine() {
    let searchEngineList = document.getElementsByClassName("searchEngine");
    let searchEngineArray = [];

    for (let i = 0; i < searchEngineList.length; i++) {
        let searchEngineName = searchEngineList.item(i).parentElement.parentElement.childNodes.item(1).textContent;
        if (i > 3) {
            searchEngineName = searchEngineList.item(i).parentElement.parentElement.childNodes.item(0).textContent;
        }
        searchEngineArray[searchEngineArray.length] = searchEngineName;
    }

    for (let i = 0; i < searchEngineList.length; i++) {
        if (searchEngineList.item(i).checked == true) {
            settingsCore.selectDefaultSearchEngine(searchEngineArray[i]);
            break;
        }
    }
}

function setSelectedDefaultSearchEngine(defaultSearchEngine) {
    let searchEngineList = document.getElementsByClassName("searchEngine");
    let searchEngineArray = [];

    for (let i = 0; i < searchEngineList.length; i++) {
        let searchEngineName = searchEngineList.item(i).parentElement.parentElement.childNodes.item(1).textContent;
        if (i > 3) {
            searchEngineName = searchEngineList.item(i).parentElement.parentElement.childNodes.item(0).textContent;
        }
        searchEngineArray[searchEngineArray.length] = searchEngineName;
    }

    for (let i = 0; i < searchEngineArray.length; i++) {
        if (searchEngineArray[i].localeCompare(defaultSearchEngine) == 0) {
            searchEngineList.item(i).checked = true;
            break;
        }
    }
}

function selectUILanguage() {
    let ololanLanguage = document.getElementById("ololanLanguage");
    settingsCore.selectUILanguage(ololanLanguage.value);
}

function setSelectedUILanguage(uiLanguageValue) {
    let ololanLanguage = document.getElementById("ololanLanguage");
    ololanLanguage.value = uiLanguageValue;
}

function selectSpellCheckLanguage() {
    let SpellCheckLanguage = document.getElementById("spellCheckLanguage");
    settingsCore.selectSpellCheckLanguage(SpellCheckLanguage.value);
}

function setSelectedSpellCheckLanguage(SpellCheckLanguageValue) {
    let SpellCheckLanguage = document.getElementById("spellCheckLanguage");
    SpellCheckLanguage.value = SpellCheckLanguageValue;
}

function selectDownloadLocation() {
    settingsCore.selectDownloadLocation();
}

function setDownloadLocation(path) {
    let downloadPath = document.getElementById("downloadPath");
    downloadPath.textContent = path;
}

function askDownloadLocation() {
    let downloadLocation = document.getElementById("downloadLocation");
    if (downloadLocation.checked == true) {
        settingsCore.askDownloadLocation("true");
    } else {
        settingsCore.askDownloadLocation("false");
    }
}

function setAskDownloadLocation(state) {
    let downloadLocation = document.getElementById("downloadLocation");
    downloadLocation.checked = state;
}

function enableOlolanAssistant() {
    let ololanAssistant = document.getElementById("ololanAssistant");
    if (ololanAssistant.checked == true) {
        settingsCore.enableOlolanAssistant("true");
    } else {
        settingsCore.enableOlolanAssistant("false");
    }
}

function setEnableOlolanAssistant(state) {
    let ololanAssistant = document.getElementById("ololanAssistant");
    ololanAssistant.checked = state;
}

function clearAssistantData() {
    settingsCore.clearAssistantData();
}

function setDefaultBrowser() {
    settingsCore.setDefaultBrowser();
}

function setDefaultBrowserState(state) {
    if (state) {
        let defaultBrowser = document.getElementById("defaultBrowser");
        defaultBrowser.style.display = "flex";
        let defBrowButton = document.getElementById("defBrowserButton");
        defBrowButton.style.display = "none";
    } else {
        let defaultBrowser = document.getElementById("defaultBrowser");
        defaultBrowser.style.display = "none";
        let defBrowButton = document.getElementById("defBrowserButton");
        defBrowButton.style.display = "flex";
    }
}

function useCanvasAcceleration() {
    let canvasAcceleration = document.getElementById("canvasAcceleration");
    if (canvasAcceleration.checked == true) {
        settingsCore.useCanvasAcceleration("true");
    } else {
        settingsCore.useCanvasAcceleration("false");
    }
}

function setUseCanvasAcceleration(state) {
    let canvasAcceleration = document.getElementById("canvasAcceleration");
    canvasAcceleration.checked = state;
}

function useWebGLAcceleration() {
    let canvasAcceleration = document.getElementById("webGLAcceleration");
    if (canvasAcceleration.checked == true) {
        settingsCore.useWebGLAcceleration("true");
    } else {
        settingsCore.useWebGLAcceleration("false");
    }
}

function setUseWebGLAcceleration(state) {
    let canvasAcceleration = document.getElementById("webGLAcceleration");
    canvasAcceleration.checked = state;
}

function manageProxy() {
    settingsCore.manageProxy();
}

function restoreSettings() {
    settingsCore.restoreSettings();
}

function updateOlolan() {
    animateUpdateChecking();
    settingsCore.updateOlolan();
}

function downloadAvailableUpdates() {
    settingsCore.downloadAvailableUpdates();
}

function showUpdateCheckingInfo(updateMsg, downloadUpdates) {
    clearInterval(updateAnimationIndex);
    while (animationArrayIndex.length > 0) {
        clearTimeout(animationArrayIndex.shift());
    }
    document.getElementById("versionText").textContent = updateMsg;

    if (downloadUpdates.localeCompare("true") == 0) {
        document.getElementById("updateCheckButton").style.display = "none";
        document.getElementById("updateDownloadButton").style.display = "flex";
    }
}

function showUpdateProgress(updateMsg) {
    document.getElementById("versionText").textContent = updateMsg;
    document.getElementById("updateCheckButton").style.display = "none";
    document.getElementById("updateDownloadButton").style.display = "none";
}

function animateUpdateChecking() {
    updateAnimationIndex = setInterval(function() {
        animationArrayIndex.push(setTimeout(function() {
            document.getElementById("versionText").textContent = "Checking for updates";
        }, 0));
        animationArrayIndex.push(setTimeout(function() {
            document.getElementById("versionText").textContent = "Checking for updates.";
        }, 350));
        animationArrayIndex.push(setTimeout(function() {
            document.getElementById("versionText").textContent = "Checking for updates...";
        }, 700));
        animationArrayIndex.push(setTimeout(function() {
            document.getElementById("versionText").textContent = "Checking for updates...";
        }, 1050));
    }, 1100);
}

function selectAboutOlolan() {
    selectedOption = "About Ololan";
    setOptionStyle("About Ololan");
    let optionFolder = document.getElementsByClassName("optionButton").item(13);
    let folderSection = document.getElementById("folderSection");
    folderSection.scrollTo(optionFolder.getBoundingClientRect().x, optionFolder.getBoundingClientRect().y);
}

function closeSEDialogBox() {
    let dialogBox = document.getElementsByClassName("dialogContainer");
    dialogBox.item(0).style.display = "none";
}

function openSEDialogBoxA() {
    let nameInput = document.getElementById("nameInput");
    nameInput.value = "";

    let shortcutInput = document.getElementById("shortcutInput");
    shortcutInput.value = "";

    let urlInput = document.getElementById("urlInput");
    urlInput.value = "";

    let dialogBoxA = document.getElementById("sEngineDialogBoxA");
    dialogBoxA.style.display = "block";

    let dialogBoxB = document.getElementById("sEngineDialogBoxB");
    dialogBoxB.style.display = "none";

    let dialogBox = document.getElementsByClassName("dialogContainer");
    dialogBox.item(0).style.display = "flex";

    nameInput.focus();
}

function openSEDialogBoxB(event) {
    let editButton = event.target;
    editingSearchEngine = editButton.parentElement;

    let nameInputEdit = document.getElementById("nameInputEdit");
    nameInputEdit.value = editButton.parentElement.childNodes.item(0).textContent;

    let shortcutInputEdit = document.getElementById("shortcutInputEdit");
    shortcutInputEdit.value = editButton.parentElement.childNodes.item(1).textContent;

    let urlInputEdit = document.getElementById("urlInputEdit");
    urlInputEdit.value = editButton.parentElement.title;

    let dialogBoxA = document.getElementById("sEngineDialogBoxA");
    dialogBoxA.style.display = "none";

    let dialogBoxB = document.getElementById("sEngineDialogBoxB");
    dialogBoxB.style.display = "block";

    let dialogBox = document.getElementsByClassName("dialogContainer");
    dialogBox.item(0).style.display = "flex";

    nameInputEdit.focus();
}

function saveSearchEngine() {
    let valueA = document.getElementById("urlInput").value;
    let valueB = document.getElementById("nameInput").value;
    let valueC = document.getElementById("shortcutInput").value;

    if ((valueA.length > 0) && (valueB.length > 0) && (valueC.length > 0)) {
        let searchEngineList = document.getElementsByClassName("searchEngine");
        let searchEngineArray = [];

        for (let i = 0; i < searchEngineList.length; i++) {
            let searchEngineName = searchEngineList.item(i).parentElement.parentElement.childNodes.item(1).textContent;
            if (i > 3) {
                searchEngineName = searchEngineList.item(i).parentElement.parentElement.childNodes.item(0).textContent;
            }
            searchEngineArray[searchEngineArray.length] = searchEngineName;
        }

        let optionRow = document.createElement("DIV");
        optionRow.className = "optionRow";
        optionRow.title = document.getElementById("urlInput").value;

        valueB = generateSearchEngineName(valueB, searchEngineArray);
        let rowDetailA = document.createElement("DIV");
        rowDetailA.className = "rowDetail";
        rowDetailA.textContent = valueB;
        rowDetailA.style.width = "47.5%";

        let rowDetailB = document.createElement("DIV");
        rowDetailB.className = "rowDetail";
        rowDetailB.textContent = document.getElementById("shortcutInput").value;
        rowDetailB.style.width = "39%";

        let checkBx = document.createElement("LABEL");
        checkBx.className = "checkBx";

        let checkBInput = document.createElement("INPUT");
        checkBInput.className = "checkBInput";
        checkBInput.classList.add("searchEngine");
        checkBInput.type = "radio";
        checkBInput.name = "radioSearch";
        checkBInput.addEventListener("click", selectDefaultSearchEngine);

        let checkmark = document.createElement("SPAN");
        checkmark.className = "checkmark";

        let spButtonEdit = document.createElement("BUTTON");
        spButtonEdit.className = "spButtonEdit";
        spButtonEdit.addEventListener("click", openSEDialogBoxB);

        let spButtonDelete = document.createElement("BUTTON");
        spButtonDelete.className = "spButtonDelete";
        spButtonDelete.addEventListener("click", deleteSearchEngine);

        checkBx.appendChild(checkBInput);
        checkBx.appendChild(checkmark);
        optionRow.appendChild(rowDetailA);
        optionRow.appendChild(rowDetailB);
        optionRow.appendChild(spButtonEdit);
        optionRow.appendChild(spButtonDelete);
        optionRow.appendChild(checkBx);
        document.getElementsByClassName("optionContainer").item(6).appendChild(optionRow);
        settingsCore.saveSearchEngine(valueB, valueC, valueA);
        closeSEDialogBox();
    }
}

function editSearchEngine() {
    let newTitle = document.getElementById("nameInputEdit").value;
    let newShortcut = document.getElementById("shortcutInputEdit").value;
    let newUrl = document.getElementById("urlInputEdit").value;

    if ((newTitle.length > 0) && (newShortcut.length > 0) && (newUrl.length > 0)) {
        let searchEngineList = document.getElementsByClassName("searchEngine");
        let searchEngineArray = [];

        for (let i = 0; i < searchEngineList.length; i++) {
            let searchEngineName = searchEngineList.item(i).parentElement.parentElement.childNodes.item(1).textContent;
            if (i > 3) {
                searchEngineName = searchEngineList.item(i).parentElement.parentElement.childNodes.item(0).textContent;
            }
            if (searchEngineName.localeCompare(editingSearchEngine.childNodes.item(0).textContent) != 0) {
                searchEngineArray[searchEngineArray.length] = searchEngineName;
            }
        }

        newTitle = generateSearchEngineName(newTitle, searchEngineArray);
        let engineTitle = editingSearchEngine.childNodes.item(0).textContent;
        editingSearchEngine.childNodes.item(0).textContent = newTitle;
        editingSearchEngine.childNodes.item(1).textContent = newShortcut;
        editingSearchEngine.title = newUrl;
        settingsCore.editSearchEngine(engineTitle, newTitle, newShortcut, newUrl);
        editingSearchEngine = null;
        closeSEDialogBox();
    }
}

function generateSearchEngineName(searchEngineName, searchEngineArray) {
    let name = searchEngineName.trim();
    let result = "";
    let occurence = 0;
    let storedOccurence = 0;
    let searchCount = 1;
    let count = 0;

    for (let i = 0; i < searchEngineArray.length; i++) {
        if (searchEngineArray[i].trim().localeCompare(name) == 0) {
            occurence++;
        }

        if ((occurence > 0) && (i == (searchEngineArray.length - 1))) {
            storedOccurence = storedOccurence + occurence;
            occurence = 0;

            if (searchCount == 1) {
                name = name + " (" + storedOccurence + ")";
                result = name;
            } else if (searchCount == (count + 1)) {
                name = searchEngineName + " (" + storedOccurence + ")";
                result = name;
            }

            i = -1;
            count = searchCount;
            searchCount++;
        }
    }

    return (result.length > 0 ? result : name);
}

function deleteSearchEngine(event) {
    let deleteButton = event.target;
    let row = deleteButton.parentElement;
    settingsCore.deleteSearchEngine(row.childNodes.item(0).textContent);
    row.remove();
}

function loadCustomSearchEngine(title, shortcut, url) {
    let optionRow = document.createElement("DIV");
    optionRow.className = "optionRow";
    optionRow.title = url;

    let rowDetailA = document.createElement("DIV");
    rowDetailA.className = "rowDetail";
    rowDetailA.textContent = title;
    rowDetailA.style.width = "47.5%";

    let rowDetailB = document.createElement("DIV");
    rowDetailB.className = "rowDetail";
    rowDetailB.textContent = shortcut;
    rowDetailB.style.width = "39%";

    let checkBx = document.createElement("LABEL");
    checkBx.className = "checkBx";

    let checkBInput = document.createElement("INPUT");
    checkBInput.className = "checkBInput";
    checkBInput.classList.add("searchEngine");
    checkBInput.type = "radio";
    checkBInput.name = "radioSearch";
    checkBInput.addEventListener("click", selectDefaultSearchEngine);

    let checkmark = document.createElement("SPAN");
    checkmark.className = "checkmark";

    let spButtonEdit = document.createElement("BUTTON");
    spButtonEdit.className = "spButtonEdit";
    spButtonEdit.addEventListener("click", openSEDialogBoxB);

    let spButtonDelete = document.createElement("BUTTON");
    spButtonDelete.className = "spButtonDelete";
    spButtonDelete.addEventListener("click", deleteSearchEngine);

    checkBx.appendChild(checkBInput);
    checkBx.appendChild(checkmark);
    optionRow.appendChild(rowDetailA);
    optionRow.appendChild(rowDetailB);
    optionRow.appendChild(spButtonEdit);
    optionRow.appendChild(spButtonDelete);
    optionRow.appendChild(checkBx);
    document.getElementsByClassName("optionContainer").item(6).appendChild(optionRow);
}