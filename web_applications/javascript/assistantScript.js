"use strict";
let assistantCore = null;
let animationIndex = 0;
let animationArray = [];
let suggested = false;
let ololanTheme = "";
let activeProcessor = "generalProcessor";

function setupConnector() {
    new QWebChannel(qt.webChannelTransport, function (ololanChannel) {
        assistantCore = ololanChannel.objects.assistantManager;
    });
}

function setActiveProcessor(processor) {
    activeProcessor = processor;
}

function setupStart() {
    let inputField = document.getElementById("searchInput");
    setFocusInStyle();
    setTimeout(function () {
        inputField.value = "";
        inputField.focus();
    }, 3000);
}

function loseFocus() {
    let inputField = document.getElementById("searchInput");
    inputField.value = "";
    inputField.blur();
}

function setFocusInStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    if (ololanTheme.localeCompare("Light Gray") == 0) {
        fieldIcon.src = "../../images/chatOn.png";
    } else if (ololanTheme.localeCompare("Light Turquoise") == 0) {
        fieldIcon.src = "../../images/chatOn.png";
    } else if (ololanTheme.localeCompare("Light Brown") == 0) {
        fieldIcon.src = "../../images/chatOnBrown.png";
    } else if (ololanTheme.localeCompare("Dark Gray") == 0) {
        fieldIcon.src = "../../images/chatOn.png";
    } else if (ololanTheme.localeCompare("Dark Turquoise") == 0) {
        fieldIcon.src = "../../images/chatOn.png";
    } else if (ololanTheme.localeCompare("Dark Brown") == 0) {
        fieldIcon.src = "../../images/chatOnBrown.png";
    } else if (ololanTheme.localeCompare("Private") == 0) {
        fieldIcon.src = "../../private_mode_images/chatOnPrivateMode.png";
    }
}

function setFocusOutStyle() {
    let fieldIcon = document.getElementById("searchBoxIcon");
    if (ololanTheme.localeCompare("Light Gray") == 0) {
        fieldIcon.src = "../../images/chatDefault.png";
    } else if (ololanTheme.localeCompare("Light Turquoise") == 0) {
        fieldIcon.src = "../../images/chatDefault.png";
    } else if (ololanTheme.localeCompare("Light Brown") == 0) {
        fieldIcon.src = "../../images/chatDefault.png";
    } else if (ololanTheme.localeCompare("Dark Gray") == 0) {
        fieldIcon.src = "../../private_mode_images/chatPrivateMode.png";
    } else if (ololanTheme.localeCompare("Dark Turquoise") == 0) {
        fieldIcon.src = "../../private_mode_images/chatPrivateMode.png";
    } else if (ololanTheme.localeCompare("Dark Brown") == 0) {
        fieldIcon.src = "../../private_mode_images/chatPrivateMode.png";
    } else if (ololanTheme.localeCompare("Private") == 0) {
        fieldIcon.src = "../../private_mode_images/chatPrivateMode.png";
    }
    if (!suggested) {
        document.getElementById("rows").innerHTML = "";
    }
}

function suggestCommands() {
    let speech = document.getElementById("searchInput").value;
    if (speech.length > 0) {
        assistantCore.suggestCommands(speech, activeProcessor);
    } else {
        emptySuggestionView();
    }
}

function showSuggestions(suggestion) {
    let itemToSuggest = document.createElement("DIV");
    itemToSuggest.className = "suggestion";
    itemToSuggest.textContent = suggestion;

    if (ololanTheme.localeCompare("Light Gray") == 0) {
        itemToSuggest.classList.add("lightTheme");
    } else if (ololanTheme.localeCompare("Light Turquoise") == 0) {
        itemToSuggest.classList.add("lightTurquoiseTheme");
    } else if ((ololanTheme.localeCompare("Light Brown") == 0)) {
        itemToSuggest.classList.add("lightBrownTheme");
    } else if (ololanTheme.localeCompare("Dark Gray") == 0) {
        itemToSuggest.classList.add("darkGrayTheme");
    } else if (ololanTheme.localeCompare("Dark Turquoise") == 0) {
        itemToSuggest.classList.add("darkTurquoiseTheme");
    } else if (ololanTheme.localeCompare("Dark Brown") == 0) {
        itemToSuggest.classList.add("darkBrownTheme");
    } else if (ololanTheme.localeCompare("Private") == 0) {
        itemToSuggest.classList.add("privateTheme");
    }

    itemToSuggest.addEventListener("click", completeSuggestion);
    document.getElementById("startView").style.display = "none";
    document.getElementById("rows").style.display = "flex";

    if (document.getElementById("searchInput").value.length > 0) {
        document.getElementById("rows").appendChild(itemToSuggest);
        suggested = true;
    } else {
        emptySuggestionView();
    }
}

function emptySuggestionView() {
    document.getElementById("rows").innerHTML = "";
    document.getElementById("rows").style.display = "none";
    document.getElementById("startView").style.display = "flex";
    suggested = false;
}

function validateInput(event) {
    if (event.key.localeCompare("Enter") == 0) {
        let speech = document.getElementById("searchInput").value;
        let speechB = speech.replace(speech[0], speech[0].toUpperCase());
        speech = speech.toLowerCase();
        document.getElementById("searchInput").value = "";

        addUserSpeech(speechB);
        setTimeout(function () { addTypingBubbles(); }, 200);
        setTimeout(function () { assistantCore.sendCommand(speech, activeProcessor); }, 300);
    } else if (event.key.localeCompare("Tab") == 0) {
        event.preventDefault();
        if (suggested) {
            document.getElementById("searchInput").value = "";
            document.getElementById("searchInput").value = document.getElementsByClassName("suggestion").item(0).textContent.replace(" - Press tab", "");
        }
    }
}

function completeSuggestion(event) {
    event.preventDefault();
    if (suggested) {
        let textCommand = event.target.textContent;
        if (textCommand.endsWith(" - Press tab")) {
            textCommand = textCommand.replace(" - Press tab", "");
        }

        document.getElementById("searchInput").focus();
        document.getElementById("searchInput").value = "";
        document.getElementById("searchInput").value = textCommand;
    }
}

function addAssistantSpeech(speech) {
    let leftBubble = document.createElement("DIV");
    leftBubble.className = "leftBubble";
    leftBubble.textContent = speech;

    if (ololanTheme.localeCompare("Light Gray") == 0) {
        leftBubble.classList.add("lightTheme");
    } else if (ololanTheme.localeCompare("Light Turquoise") == 0) {
        leftBubble.classList.add("lightTheme");
    } else if (ololanTheme.localeCompare("Light Brown") == 0) {
        leftBubble.classList.add("lightTheme");
    } else if (ololanTheme.localeCompare("Dark Gray") == 0) {
        leftBubble.classList.add("darkGrayTheme");
    } else if (ololanTheme.localeCompare("Dark Turquoise") == 0) {
        leftBubble.classList.add("darkTurquoiseTheme");
    } else if (ololanTheme.localeCompare("Dark Brown") == 0) {
        leftBubble.classList.add("darkBrownTheme");
    } else if (ololanTheme.localeCompare("Private") == 0) {
        leftBubble.classList.add("privateTheme");
    }

    let leftRow = document.createElement("DIV");
    leftRow.className = "leftRow";
    leftRow.appendChild(leftBubble);

    let chatView = document.getElementById("chatView");
    setTimeout(function () {
        endTyping();
        setTimeout(function () {
            chatView.appendChild(leftRow);
            chatView.scrollBy(0, chatView.clientHeight);
            leftBubble.classList.add("loaded");
            setTimeout(function () { leftBubble.classList.add("afterLoad"); }, 101);
            setTimeout(function () { chatView.scrollBy(0, chatView.clientHeight); }, 102);
        }, 300);
    }, 700);
}

function addUserSpeech(speech) {
    endTyping();
    let rightBubble = document.createElement("DIV");
    rightBubble.className = "rightBubble";
    rightBubble.textContent = speech;

    if (ololanTheme.localeCompare("Light Gray") == 0) {
        rightBubble.classList.add("defaultLightTheme");
    } else if (ololanTheme.localeCompare("Light Turquoise") == 0) {
        rightBubble.classList.add("defaultLightTheme");
    } else if (ololanTheme.localeCompare("Light Brown") == 0) {
        rightBubble.classList.add("lightBrownTheme");
    } else if (ololanTheme.localeCompare("Dark Gray") == 0) {
        rightBubble.classList.add("defaultDarkTheme");
    } else if (ololanTheme.localeCompare("Dark Turquoise") == 0) {
        rightBubble.classList.add("defaultDarkTheme");
    } else if (ololanTheme.localeCompare("Dark Brown") == 0) {
        rightBubble.classList.add("darkBrownTheme");
    } else if (ololanTheme.localeCompare("Private") == 0) {
        rightBubble.classList.add("privateTheme");
    }

    let rightRow = document.createElement("DIV");
    rightRow.className = "rightRow";
    rightRow.appendChild(rightBubble);

    let chatView = document.getElementById("chatView");
    setTimeout(function () {
        chatView.appendChild(rightRow);
        chatView.scrollBy(0, chatView.clientHeight);
        rightBubble.classList.add("loaded");
        setTimeout(function () { rightBubble.classList.add("afterLoad"); }, 101);
        setTimeout(function () { chatView.scrollBy(0, chatView.clientHeight); }, 102);
    }, 101);
}

function addTypingBubbles() {
    let bubbleArea = document.createElement("DIV");
    bubbleArea.id = "bubbleArea";

    let leftBubble = document.createElement("DIV");
    leftBubble.className = "leftBubble";
    leftBubble.appendChild(bubbleArea);

    if (ololanTheme.localeCompare("Light Gray") == 0) {
        leftBubble.classList.add("lightTheme");
    } else if (ololanTheme.localeCompare("Light Turquoise") == 0) {
        leftBubble.classList.add("lightTheme");
    } else if (ololanTheme.localeCompare("Light Brown") == 0) {
        leftBubble.classList.add("lightTheme");
    } else if (ololanTheme.localeCompare("Dark Gray") == 0) {
        leftBubble.classList.add("darkGrayTheme");
    } else if (ololanTheme.localeCompare("Dark Turquoise") == 0) {
        leftBubble.classList.add("darkTurquoiseTheme");
    } else if (ololanTheme.localeCompare("Dark Brown") == 0) {
        leftBubble.classList.add("darkBrownTheme");
    } else if (ololanTheme.localeCompare("Private") == 0) {
        leftBubble.classList.add("privateTheme");
    }

    for (let i = 0; i < 3; i++) {
        let bubble = document.createElement("DIV");
        bubble.className = "typingBubble";
        bubbleArea.appendChild(bubble);
    }

    let leftRow = document.createElement("DIV");
    leftRow.className = "leftRow";
    leftRow.appendChild(leftBubble);

    let chatView = document.getElementById("chatView");
    setTimeout(function () {
        chatView.appendChild(leftRow);
        chatView.scrollBy(0, chatView.clientHeight);
        leftBubble.classList.add("loaded");
        setTimeout(function () { chatView.scrollBy(0, chatView.clientHeight); }, 101);
        startTyping();
    }, 101);
}

function startTyping() {
    let bubbleList = document.getElementsByClassName("typingBubble");
    let onTheme = "onLightTheme";
    let offTheme = "offLightTheme";

    if ((ololanTheme.localeCompare("Light Gray") == 0) || (ololanTheme.localeCompare("Light Brown") == 0) ||
        (ololanTheme.localeCompare("Light Turquoise") == 0)) {
        onTheme = "onLightTheme";
        offTheme = "offLightTheme";
    } else if (ololanTheme.localeCompare("Dark Gray") == 0) {
        onTheme = "onDarkGrayTheme";
        offTheme = "offDarkGrayTheme";
    } else if (ololanTheme.localeCompare("Dark Turquoise") == 0) {
        onTheme = "onDarkTurquoiseTheme";
        offTheme = "offDarkTurquoiseTheme";
    } else if (ololanTheme.localeCompare("Dark Brown") == 0) {
        onTheme = "onDarkBrownTheme";
        offTheme = "offDarkBrownTheme";
    } else if (ololanTheme.localeCompare("Private") == 0) {
        onTheme = "onPrivateTheme";
        offTheme = "offPrivateTheme";
    }

    animationArray.push(setTimeout(function () {
        bubbleList.item(0).classList.add(onTheme);
        bubbleList.item(0).classList.remove(offTheme);
    }, 0));
    animationArray.push(setTimeout(function () {
        bubbleList.item(0).classList.add(offTheme);
        bubbleList.item(0).classList.remove(onTheme);
    }, 350));
    animationArray.push(setTimeout(function () {
        bubbleList.item(1).classList.add(onTheme);
        bubbleList.item(1).classList.remove(offTheme);
    }, 190));
    animationArray.push(setTimeout(function () {
        bubbleList.item(1).classList.add(offTheme);
        bubbleList.item(1).classList.remove(onTheme);
    }, 525));
    animationArray.push(setTimeout(function () {
        bubbleList.item(2).classList.add(onTheme);
        bubbleList.item(2).classList.remove(offTheme);
    }, 365));
    animationArray.push(setTimeout(function () {
        bubbleList.item(2).classList.add(offTheme);
        bubbleList.item(2).classList.remove(onTheme);
    }, 700));

    animationIndex = setInterval(function () {
        animationArray.push(setTimeout(function () {
            bubbleList.item(0).classList.add(onTheme);
            bubbleList.item(0).classList.remove(offTheme);
        }, 0));
        animationArray.push(setTimeout(function () {
            bubbleList.item(0).classList.add(offTheme);
            bubbleList.item(0).classList.remove(onTheme);
        }, 350));
        animationArray.push(setTimeout(function () {
            bubbleList.item(1).classList.add(onTheme);
            bubbleList.item(1).classList.remove(offTheme);
        }, 190));
        animationArray.push(setTimeout(function () {
            bubbleList.item(1).classList.add(offTheme);
            bubbleList.item(1).classList.remove(onTheme);
        }, 525));
        animationArray.push(setTimeout(function () {
            bubbleList.item(2).classList.add(onTheme);
            bubbleList.item(2).classList.remove(offTheme);
        }, 365));
        animationArray.push(setTimeout(function () {
            bubbleList.item(2).classList.add(offTheme);
            bubbleList.item(2).classList.remove(onTheme);
        }, 700));
    }, 800);
}

function endTyping() {
    if (animationIndex != 0) {
        clearInterval(animationIndex);
        animationIndex = 0;

        while (animationArray.length > 0) {
            clearTimeout(animationArray.shift());
        }

        let leftRowList = document.getElementsByClassName("leftRow");
        leftRowList.item(leftRowList.length - 1).remove();
    }
}

function clearChatView() {
    let chatView = document.getElementById("chatView");
    endTyping();
    chatView.innerHTML = "";
}

function setLightGrayTheme() {
    let chatView = document.getElementById("chatView");
    chatView.classList.remove("privateTheme");
    chatView.classList.remove("darkBrownTheme");
    chatView.classList.remove("darkTurquoiseTheme");
    chatView.classList.remove("darkGrayTheme");
    chatView.classList.add("lightTheme");

    let searchBox = document.getElementById("searchBox");
    searchBox.classList.remove("privateTheme");
    searchBox.classList.remove("darkBrownTheme");
    searchBox.classList.remove("darkTurquoiseTheme");
    searchBox.classList.remove("darkGrayTheme");
    searchBox.classList.remove("lightBrownTheme");
    searchBox.classList.remove("lightTurquoiseTheme");
    searchBox.classList.add("defaultLightTheme");

    let searchInput = document.getElementById("searchInput");
    searchInput.classList.remove("darkTheme");
    searchInput.classList.add("lightTheme");

    let leftBubble = document.getElementsByClassName("leftBubble");
    for (let i = 0; i < leftBubble.length; i++) {
        leftBubble.item(i).classList.remove("privateTheme");
        leftBubble.item(i).classList.remove("darkBrownTheme");
        leftBubble.item(i).classList.remove("darkTurquoiseTheme");
        leftBubble.item(i).classList.remove("darkGrayTheme");
        leftBubble.item(i).classList.add("lightTheme");
    }

    let rightBubble = document.getElementsByClassName("rightBubble");
    for (let i = 0; i < rightBubble.length; i++) {
        rightBubble.item(i).classList.remove("privateTheme");
        rightBubble.item(i).classList.remove("darkBrownTheme");
        rightBubble.item(i).classList.remove("defaultDarkTheme");
        rightBubble.item(i).classList.remove("lightBrownTheme");
        rightBubble.item(i).classList.add("defaultLightTheme");
    }

    let suggestion = document.getElementsByClassName("suggestion");
    for (let i = 0; i < suggestion.length; i++) {
        suggestion.item(i).classList.remove("privateTheme");
        suggestion.item(i).classList.remove("darkBrownTheme");
        suggestion.item(i).classList.remove("darkTurquoiseTheme");
        suggestion.item(i).classList.remove("darkGrayTheme");
        suggestion.item(i).classList.remove("lightTurquoiseTheme");
        suggestion.item(i).classList.remove("lightBrownTheme");
        suggestion.item(i).classList.add("lightTheme");
    }

    document.body.style.backgroundColor = "#ffffff";
    document.body.style.border = "1px solid #d1d1d3";
    document.getElementById("suggestionView").style.backgroundColor = "#ffffff";
    document.getElementById("startView").style.color = "#656565";
    ololanTheme = "Light Gray";

    setFocusOutStyle();
}

function setLightTurquoiseTheme() {
    let chatView = document.getElementById("chatView");
    chatView.classList.remove("privateTheme");
    chatView.classList.remove("darkBrownTheme");
    chatView.classList.remove("darkTurquoiseTheme");
    chatView.classList.remove("darkGrayTheme");
    chatView.classList.add("lightTheme");

    let searchBox = document.getElementById("searchBox");
    searchBox.classList.remove("privateTheme");
    searchBox.classList.remove("darkBrownTheme");
    searchBox.classList.remove("darkTurquoiseTheme");
    searchBox.classList.remove("darkGrayTheme");
    searchBox.classList.remove("lightBrownTheme");
    searchBox.classList.remove("defaultLightTheme");
    searchBox.classList.add("lightTurquoiseTheme");

    let searchInput = document.getElementById("searchInput");
    searchInput.classList.remove("darkTheme");
    searchInput.classList.add("lightTheme");

    let leftBubble = document.getElementsByClassName("leftBubble");
    for (let i = 0; i < leftBubble.length; i++) {
        leftBubble.item(i).classList.remove("privateTheme");
        leftBubble.item(i).classList.remove("darkBrownTheme");
        leftBubble.item(i).classList.remove("darkTurquoiseTheme");
        leftBubble.item(i).classList.remove("darkGrayTheme");
        leftBubble.item(i).classList.add("lightTheme");
    }

    let rightBubble = document.getElementsByClassName("rightBubble");
    for (let i = 0; i < rightBubble.length; i++) {
        rightBubble.item(i).classList.remove("privateTheme");
        rightBubble.item(i).classList.remove("darkBrownTheme");
        rightBubble.item(i).classList.remove("defaultDarkTheme");
        rightBubble.item(i).classList.remove("lightBrownTheme");
        rightBubble.item(i).classList.add("defaultLightTheme");
    }

    let suggestion = document.getElementsByClassName("suggestion");
    for (let i = 0; i < suggestion.length; i++) {
        suggestion.item(i).classList.remove("privateTheme");
        suggestion.item(i).classList.remove("darkBrownTheme");
        suggestion.item(i).classList.remove("darkTurquoiseTheme");
        suggestion.item(i).classList.remove("darkGrayTheme");
        suggestion.item(i).classList.remove("lightTheme");
        suggestion.item(i).classList.remove("lightBrownTheme");
        suggestion.item(i).classList.add("lightTurquoiseTheme");
    }

    document.body.style.backgroundColor = "#ffffff";
    document.body.style.border = "1px solid #d1d1d3";
    document.getElementById("suggestionView").style.backgroundColor = "#ffffff";
    document.getElementById("startView").style.color = "#656565";
    ololanTheme = "Light Turquoise";

    setFocusOutStyle();
}

function setLightBrownTheme() {
    let chatView = document.getElementById("chatView");
    chatView.classList.remove("privateTheme");
    chatView.classList.remove("darkBrownTheme");
    chatView.classList.remove("darkTurquoiseTheme");
    chatView.classList.remove("darkGrayTheme");
    chatView.classList.add("lightTheme");

    let searchBox = document.getElementById("searchBox");
    searchBox.classList.remove("privateTheme");
    searchBox.classList.remove("darkBrownTheme");
    searchBox.classList.remove("darkTurquoiseTheme");
    searchBox.classList.remove("darkGrayTheme");
    searchBox.classList.remove("defaultLightTheme");
    searchBox.classList.remove("lightTurquoiseTheme");
    searchBox.classList.add("lightBrownTheme");

    let searchInput = document.getElementById("searchInput");
    searchInput.classList.remove("darkTheme");
    searchInput.classList.add("lightTheme");

    let leftBubble = document.getElementsByClassName("leftBubble");
    for (let i = 0; i < leftBubble.length; i++) {
        leftBubble.item(i).classList.remove("privateTheme");
        leftBubble.item(i).classList.remove("darkBrownTheme");
        leftBubble.item(i).classList.remove("darkTurquoiseTheme");
        leftBubble.item(i).classList.remove("darkGrayTheme");
        leftBubble.item(i).classList.add("lightTheme");
    }

    let rightBubble = document.getElementsByClassName("rightBubble");
    for (let i = 0; i < rightBubble.length; i++) {
        rightBubble.item(i).classList.remove("privateTheme");
        rightBubble.item(i).classList.remove("darkBrownTheme");
        rightBubble.item(i).classList.remove("defaultDarkTheme");
        rightBubble.item(i).classList.remove("defaultLightTheme");
        rightBubble.item(i).classList.add("lightBrownTheme");
    }

    let suggestion = document.getElementsByClassName("suggestion");
    for (let i = 0; i < suggestion.length; i++) {
        suggestion.item(i).classList.remove("privateTheme");
        suggestion.item(i).classList.remove("darkBrownTheme");
        suggestion.item(i).classList.remove("darkTurquoiseTheme");
        suggestion.item(i).classList.remove("darkGrayTheme");
        suggestion.item(i).classList.remove("lightTurquoiseTheme");
        suggestion.item(i).classList.remove("lightTheme");
        suggestion.item(i).classList.add("lightBrownTheme");
    }

    document.body.style.backgroundColor = "#ffffff";
    document.body.style.border = "1px solid #d1d1d3";
    document.getElementById("suggestionView").style.backgroundColor = "#ffffff";
    document.getElementById("startView").style.color = "#656565";
    ololanTheme = "Light Brown";

    setFocusOutStyle();
}

function setDarkGrayTheme() {
    let chatView = document.getElementById("chatView");
    chatView.classList.remove("privateTheme");
    chatView.classList.remove("darkBrownTheme");
    chatView.classList.remove("darkTurquoiseTheme");
    chatView.classList.remove("lightTheme");
    chatView.classList.add("darkGrayTheme");

    let searchBox = document.getElementById("searchBox");
    searchBox.classList.remove("privateTheme");
    searchBox.classList.remove("darkBrownTheme");
    searchBox.classList.remove("darkTurquoiseTheme");
    searchBox.classList.remove("lightBrownTheme");
    searchBox.classList.remove("defaultLightTheme");
    searchBox.classList.remove("lightTurquoiseTheme");
    searchBox.classList.add("darkGrayTheme");

    let searchInput = document.getElementById("searchInput");
    searchInput.classList.remove("lightTheme");
    searchInput.classList.add("darkTheme");

    let leftBubble = document.getElementsByClassName("leftBubble");
    for (let i = 0; i < leftBubble.length; i++) {
        leftBubble.item(i).classList.remove("privateTheme");
        leftBubble.item(i).classList.remove("darkBrownTheme");
        leftBubble.item(i).classList.remove("darkTurquoiseTheme");
        leftBubble.item(i).classList.remove("lightTheme");
        leftBubble.item(i).classList.add("darkGrayTheme");
    }

    let rightBubble = document.getElementsByClassName("rightBubble");
    for (let i = 0; i < rightBubble.length; i++) {
        rightBubble.item(i).classList.remove("privateTheme");
        rightBubble.item(i).classList.remove("darkBrownTheme");
        rightBubble.item(i).classList.remove("lightBrownTheme");
        rightBubble.item(i).classList.remove("defaultLightTheme");
        rightBubble.item(i).classList.add("defaultDarkTheme");
    }

    let suggestion = document.getElementsByClassName("suggestion");
    for (let i = 0; i < suggestion.length; i++) {
        suggestion.item(i).classList.remove("privateTheme");
        suggestion.item(i).classList.remove("darkBrownTheme");
        suggestion.item(i).classList.remove("darkTurquoiseTheme");
        suggestion.item(i).classList.remove("lightTheme");
        suggestion.item(i).classList.remove("lightTurquoiseTheme");
        suggestion.item(i).classList.remove("lightBrownTheme");
        suggestion.item(i).classList.add("darkGrayTheme");
    }

    document.body.style.backgroundColor = "#434344";
    document.body.style.border = "1px solid #6b6b6b";
    document.getElementById("suggestionView").style.backgroundColor = "#434344";
    document.getElementById("startView").style.color = "#ffffff";
    ololanTheme = "Dark Gray";

    setFocusOutStyle();
}

function setDarkTurquoiseTheme() {
    let chatView = document.getElementById("chatView");
    chatView.classList.remove("privateTheme");
    chatView.classList.remove("darkBrownTheme");
    chatView.classList.remove("lightTheme");
    chatView.classList.remove("darkGrayTheme");
    chatView.classList.add("darkTurquoiseTheme");

    let searchBox = document.getElementById("searchBox");
    searchBox.classList.remove("privateTheme");
    searchBox.classList.remove("darkBrownTheme");
    searchBox.classList.remove("lightBrownTheme");
    searchBox.classList.remove("darkGrayTheme");
    searchBox.classList.remove("defaultLightTheme");
    searchBox.classList.remove("lightTurquoiseTheme");
    searchBox.classList.add("darkTurquoiseTheme");

    let searchInput = document.getElementById("searchInput");
    searchInput.classList.remove("lightTheme");
    searchInput.classList.add("darkTheme");

    let leftBubble = document.getElementsByClassName("leftBubble");
    for (let i = 0; i < leftBubble.length; i++) {
        leftBubble.item(i).classList.remove("privateTheme");
        leftBubble.item(i).classList.remove("darkBrownTheme");
        leftBubble.item(i).classList.remove("lightTheme");
        leftBubble.item(i).classList.remove("darkGrayTheme");
        leftBubble.item(i).classList.add("darkTurquoiseTheme");
    }

    let rightBubble = document.getElementsByClassName("rightBubble");
    for (let i = 0; i < rightBubble.length; i++) {
        rightBubble.item(i).classList.remove("privateTheme");
        rightBubble.item(i).classList.remove("darkBrownTheme");
        rightBubble.item(i).classList.remove("lightBrownTheme");
        rightBubble.item(i).classList.remove("defaultLightTheme");
        rightBubble.item(i).classList.add("defaultDarkTheme");
    }

    let suggestion = document.getElementsByClassName("suggestion");
    for (let i = 0; i < suggestion.length; i++) {
        suggestion.item(i).classList.remove("privateTheme");
        suggestion.item(i).classList.remove("darkBrownTheme");
        suggestion.item(i).classList.remove("lightTheme");
        suggestion.item(i).classList.remove("darkGrayTheme");
        suggestion.item(i).classList.remove("lightTurquoiseTheme");
        suggestion.item(i).classList.remove("lightBrownTheme");
        suggestion.item(i).classList.add("darkTurquoiseTheme");
    }

    document.body.style.backgroundColor = "#1e4446";
    document.body.style.border = "1px solid #2e686b";
    document.getElementById("suggestionView").style.backgroundColor = "#1e4446";
    document.getElementById("startView").style.color = "#ffffff";
    ololanTheme = "Dark Turquoise";

    setFocusOutStyle();
}

function setDarkBrownTheme() {
    let chatView = document.getElementById("chatView");
    chatView.classList.remove("privateTheme");
    chatView.classList.remove("lightTheme");
    chatView.classList.remove("darkTurquoiseTheme");
    chatView.classList.remove("darkGrayTheme");
    chatView.classList.add("darkBrownTheme");

    let searchBox = document.getElementById("searchBox");
    searchBox.classList.remove("privateTheme");
    searchBox.classList.remove("lightBrownTheme");
    searchBox.classList.remove("darkTurquoiseTheme");
    searchBox.classList.remove("darkGrayTheme");
    searchBox.classList.remove("defaultLightTheme");
    searchBox.classList.remove("lightTurquoiseTheme");
    searchBox.classList.add("darkBrownTheme");

    let searchInput = document.getElementById("searchInput");
    searchInput.classList.remove("lightTheme");
    searchInput.classList.add("darkTheme");

    let leftBubble = document.getElementsByClassName("leftBubble");
    for (let i = 0; i < leftBubble.length; i++) {
        leftBubble.item(i).classList.remove("privateTheme");
        leftBubble.item(i).classList.remove("lightTheme");
        leftBubble.item(i).classList.remove("darkTurquoiseTheme");
        leftBubble.item(i).classList.remove("darkGrayTheme");
        leftBubble.item(i).classList.add("darkBrownTheme");
    }

    let rightBubble = document.getElementsByClassName("rightBubble");
    for (let i = 0; i < rightBubble.length; i++) {
        rightBubble.item(i).classList.remove("privateTheme");
        rightBubble.item(i).classList.remove("lightBrownTheme");
        rightBubble.item(i).classList.remove("defaultDarkTheme");
        rightBubble.item(i).classList.remove("defaultLightTheme");
        rightBubble.item(i).classList.add("darkBrownTheme");
    }

    let suggestion = document.getElementsByClassName("suggestion");
    for (let i = 0; i < suggestion.length; i++) {
        suggestion.item(i).classList.remove("privateTheme");
        suggestion.item(i).classList.remove("lightTheme");
        suggestion.item(i).classList.remove("darkTurquoiseTheme");
        suggestion.item(i).classList.remove("darkGrayTheme");
        suggestion.item(i).classList.remove("lightTurquoiseTheme");
        suggestion.item(i).classList.remove("lightBrownTheme");
        suggestion.item(i).classList.add("darkBrownTheme");
    }

    document.body.style.backgroundColor = "#462c21";
    document.body.style.border = "1px solid #6b4232";
    document.getElementById("suggestionView").style.backgroundColor = "#462c21";
    document.getElementById("startView").style.color = "#ffffff";
    ololanTheme = "Dark Brown";

    setFocusOutStyle();
}

function setPrivateTheme() {
    let chatView = document.getElementById("chatView");
    chatView.classList.remove("lightTheme");
    chatView.classList.remove("darkBrownTheme");
    chatView.classList.remove("darkTurquoiseTheme");
    chatView.classList.remove("darkGrayTheme");
    chatView.classList.add("privateTheme");

    let searchBox = document.getElementById("searchBox");
    searchBox.classList.remove("lightBrownTheme");
    searchBox.classList.remove("darkBrownTheme");
    searchBox.classList.remove("darkTurquoiseTheme");
    searchBox.classList.remove("darkGrayTheme");
    searchBox.classList.remove("defaultLightTheme");
    searchBox.classList.remove("lightTurquoiseTheme");
    searchBox.classList.add("privateTheme");

    let searchInput = document.getElementById("searchInput");
    searchInput.classList.remove("lightTheme");
    searchInput.classList.add("darkTheme");

    let leftBubble = document.getElementsByClassName("leftBubble");
    for (let i = 0; i < leftBubble.length; i++) {
        leftBubble.item(i).classList.remove("lightTheme");
        leftBubble.item(i).classList.remove("darkBrownTheme");
        leftBubble.item(i).classList.remove("darkTurquoiseTheme");
        leftBubble.item(i).classList.remove("darkGrayTheme");
        leftBubble.item(i).classList.add("privateTheme");
    }

    let rightBubble = document.getElementsByClassName("rightBubble");
    for (let i = 0; i < rightBubble.length; i++) {
        rightBubble.item(i).classList.remove("lightBrownTheme");
        rightBubble.item(i).classList.remove("darkBrownTheme");
        rightBubble.item(i).classList.remove("defaultDarkTheme");
        rightBubble.item(i).classList.remove("defaultLightTheme");
        rightBubble.item(i).classList.add("privateTheme");
    }

    let suggestion = document.getElementsByClassName("suggestion");
    for (let i = 0; i < suggestion.length; i++) {
        suggestion.item(i).classList.remove("lightTheme");
        suggestion.item(i).classList.remove("darkBrownTheme");
        suggestion.item(i).classList.remove("darkTurquoiseTheme");
        suggestion.item(i).classList.remove("darkGrayTheme");
        suggestion.item(i).classList.remove("lightTurquoiseTheme");
        suggestion.item(i).classList.remove("lightBrownTheme");
        suggestion.item(i).classList.add("privateTheme");
    }

    document.body.style.backgroundColor = "#3c3244";
    document.body.style.border = "1px solid #5e4e6b";
    document.getElementById("suggestionView").style.backgroundColor = "#3c3244";
    document.getElementById("startView").style.color = "#ffffff";
    ololanTheme = "Private";

    setFocusOutStyle();
}