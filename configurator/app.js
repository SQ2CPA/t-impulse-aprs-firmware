let port, reader, writer;

const IDs = { usbProductId: 22336, usbVendorId: 1155 };

const encoder = new TextEncoder(),
    decoder = new TextDecoder();

const BUFFER_SIZE = 62;

async function readBytes(numberOfBytes) {
    const bytes = new Uint8Array(numberOfBytes);
    let readBytes = 0,
        finished = false;

    try {
        while (true) {
            const { value } = await reader.read();

            if (value) {
                for (let i = 0; i < value.length; i++) {
                    bytes[readBytes] = value[i];
                    readBytes++;

                    if (readBytes >= numberOfBytes) {
                        finished = true;
                    }
                }
            }

            if (finished) break;
        }
    } catch (err) {
        throw err;
    }

    return bytes;
}

$(document).ready(function () {
    $('*[data-button="connect"]').on("click", async function () {
        port = await navigator.serial.requestPort();

        const info = port.getInfo();

        if (
            info.usbProductId !== IDs.usbProductId ||
            info.usbVendorId !== IDs.usbVendorId
        ) {
            alert(`Selected device is NOT the T-Impulse! Please try again.`);
            return;
        }

        await port.open({ baudRate: 115200 });

        $(this).attr("disabled", true);
        $('*[data-id="status"]')
            .html("<i class='fa-solid fa-check'></i> Connecting</a>")
            .css("color", "orange");

        reader = port.readable.getReader();
        writer = port.writable.getWriter();

        await writer.write(new Uint8Array([0x01, 0x10]));

        await new Promise((r) => setTimeout(r, 1000));

        const result = await Promise.race([
            new Promise((r) => setTimeout(() => r(null), 1000)),
            reader.read(),
        ]);

        if (!result) {
            alert(`Connecting to device failed: timeout`);
            return;
        }

        const { value } = result;

        if (value[0] !== 0x01 || value[1] !== 0x10) {
            alert(`Connecting to device failed: invalid data`);
            console.error(value);
            return;
        }

        const isConfigured = value[2] === 0x02;

        if (!isConfigured) {
            $("#clear").removeClass("d-none");
        }

        const version = value[3];

        $(
            'button[data-button="save"], button[data-button="backup"], button[data-button="restore"]'
        ).attr("disabled", false);
        $("#connect").addClass("d-none");

        if (isConfigured) {
            $('*[data-id="status"]')
                .html("<i class='fa-solid fa-check'></i> Fetching config</a>")
                .css("color", "orange");

            await writer.write(new Uint8Array([0x01, 0x11]));

            await readBytes(2);

            const data = await readBytes(BUFFER_SIZE);

            console.debug(data);

            const response = new DataView(data.buffer);

            const callsign = decoder
                .decode(data.slice(0, 12))
                .replace(/\u0000/g, "");
            const path = decoder
                .decode(data.slice(12, 31))
                .replace(/\u0000/g, "");

            const overlay = decoder.decode(data.slice(31, 32));
            const symbol = decoder.decode(data.slice(32, 33));

            const comment = decoder
                .decode(data.slice(33, 58))
                .replace(/\u0000/g, "");

            const interval = response.getUint16(58, true);
            const vibration = response.getUint8(60) === 1;
            const batteryInfo = response.getUint8(61) === 1;

            $("input[name=aprs-callsign").val(callsign);
            $("input[name=aprs-path").val(path);
            $("input[name=aprs-overlay").val(overlay);
            $("input[name=aprs-symbol").val(symbol);
            $("input[name=aprs-comment").val(comment);
            $("input[name=aprs-interval").val(interval);
            $("input[name=aprs-vibration").attr("checked", vibration);
            $("input[name=aprs-battery-info").attr("checked", batteryInfo);

            $('*[data-id="status"]')
                .html(
                    `<i class='fa-solid fa-check'></i> Connected to Wristband ${
                        version === 0x01 ? "V1.0" : "unknown version"
                    }</a>`
                )
                .css("color", "lightgreen");
        } else {
            $('*[data-id="status"]')
                .html(
                    `<i class='fa-solid fa-check'></i> Connected to Wristband</a>`
                )
                .css("color", "lightgreen");
        }

        $(
            "input[type=text], input[type=number], input[type=checkbox], select"
        ).attr("disabled", false);
    });

    $("form").on("submit", async function (e) {
        e.preventDefault();

        $(
            'button[data-button="save"], button[data-button="backup"], button[data-button="restore"]'
        ).attr("disabled", true);

        $('*[data-id="status"]')
            .html("<i class='fa-solid fa-check'></i> Sending config</a>")
            .css("color", "orange");

        await writer.write(new Uint8Array([0x01, 0x12]));

        await readBytes(2);

        const callsign = $("input[name=aprs-callsign]").val();
        const path = $("input[name=aprs-path]").val();
        const overlay = $("input[name=aprs-overlay]").val();
        const symbol = $("input[name=aprs-symbol]").val();
        const comment = $("input[name=aprs-comment]").val();
        const interval = Number($("input[name=aprs-interval").val());
        const vibration = $("input[name=aprs-vibration").is(":checked");
        const batteryInfo = $("input[name=aprs-battery-info").is(":checked");

        const dataBuffer = new ArrayBuffer(BUFFER_SIZE);
        const dataView = new DataView(dataBuffer);

        for (let i = 0; i < callsign.length; i++) {
            dataView.setUint8(0 + i, callsign.charCodeAt(i)); // 0 - 12
        }

        for (let i = 0; i < path.length; i++) {
            dataView.setUint8(12 + i, path.charCodeAt(i)); // 12 - 31
        }

        dataView.setUint8(31, overlay.charCodeAt(0)); // 31 - 32
        dataView.setUint8(32, symbol.charCodeAt(0)); // 32 - 33

        for (let i = 0; i < comment.length; i++) {
            dataView.setUint8(33 + i, comment.charCodeAt(i)); // 33 - 58
        }

        dataView.setUint16(58, interval, true); // 58 - 60
        dataView.setUint8(60, vibration ? 1 : 0); // 60 - 61
        dataView.setUint8(61, batteryInfo ? 1 : 0); // 61 - 62

        console.debug(new Uint8Array(dataBuffer));

        await writer.write(dataBuffer);

        await readBytes(1);

        await writer.write(new Uint8Array([0x01, 0xff]));

        $('*[data-id="status"]')
            .html('<i class="fa-solid fa-times"></i> Device disconnected</a>')
            .css("color", "red");

        $("#saved").removeClass("d-none");
        $("#clear").addClass("d-none");

        $(
            "input[type=text], input[type=number], input[type=checkbox], select"
        ).attr("disabled", true);
    });

    $('*[data-button="backup"]').on("click", function () {
        const data = {
            callsign: $("input[name=aprs-callsign]").val(),
            path: $("input[name=aprs-path]").val(),
            overlay: $("input[name=aprs-overlay]").val(),
            symbol: $("input[name=aprs-symbol]").val(),
            comment: $("input[name=aprs-comment]").val(),
            interval: Number($("input[name=aprs-interval").val()),
            vibration: $("input[name=aprs-vibration").is(":checked"),
            batteryInfo: $("input[name=aprs-battery-info").is(":checked"),
        };

        const bb = new Blob([JSON.stringify(data)], { type: "text/json" });
        const a = document.createElement("a");
        a.download = "aprs-wristband-backup.json";
        a.href = window.URL.createObjectURL(bb);
        a.click();
    });

    $('*[data-button="restore"]').on("click", function () {
        $("input[type=file]").click();
    });

    $("input[type=file]").on("change", function () {
        const files = $(this).prop("files");

        if (!files.length) return;

        const file = files.item(0);

        const reader = new FileReader();
        reader.readAsText(file);
        reader.onload = () => {
            const data = JSON.parse(reader.result);

            $("input[name=aprs-callsign").val(data.callsign);
            $("input[name=aprs-path").val(data.path);
            $("input[name=aprs-overlay").val(data.overlay);
            $("input[name=aprs-symbol").val(data.symbol);
            $("input[name=aprs-comment").val(data.comment);
            $("input[name=aprs-interval").val(data.interval);
            $("input[name=aprs-vibration").attr("checked", data.vibration);
            $("input[name=aprs-battery-info").attr("checked", data.batteryInfo);
        };
    });

    $("#icon").on("change", function () {
        const value = $(this).find(":selected").val();

        $("input[name=aprs-overlay").val(value[0]);
        $("input[name=aprs-symbol").val(value[1]);
    });
});
