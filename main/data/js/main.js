var connection = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);

connection.onopen = function () {
	connection.send('Received from Client');
	console.log('Connected');
};

connection.onerror = function (error) {
	console.log('WebSocket Error', error);
};

connection.onmessage = function (e) {
	console.log('Received from server: ', e.data);
	processReceived(e.data);
};

connection.onclose = function () {
	console.log('WebSocket connection closed');
};

/*
*	Recivo desde servidor
*
*/
function processReceived(data) {

	json = JSON.parse(data);
	if (json.command == 'main_load')				// OK
		load_main(json);
	else if (json.command == 'detail_task')			// OK
		load_task(json);
	else if (json.command == 'set_clock_result')	// OK
		result_clock(json);
	else if (json.command == 'std_status_result')	// OK
		result_std_status(json);
	else if (json.command == 'set_task_result')		// OK
		set_task_result(json);
	else if (json.command == 'rm_task_result')		// OK
		rm_task_result(json);
	//else if (json.command == 'responce_scan_wifi')
	//	reload_table(json);
	else
		alert("Call to a function not established");
}

//---------------------------

/*
 * Metodos AUXILIARES
 * Devuelve la solo la fecha.
 * OK - 20/11/16
 */
function getDate(dateTime) {
	let DateAndTime = dateTime.split(" ");
	if (DateAndTime.length == 2) {
		let Date = DateAndTime[0].split("/");
		if (Date.length == 3) {
			return Date[2] + '/' + Date[1] + '/' + Date[0];
		}
		else
			return "Invalid format (Date)";
	}
	else
		return "Invalid format (Datetime)";
}

/*
 * Metodos AUXILIARES
 * Devuelve la solo la hora.
 * OK - 20/11/16
 */
function getTime(dateTime) {
	let DateAndTime = dateTime.split(" ");
	if (DateAndTime.length == 2) {
		let Time = DateAndTime[1].split(":");
		if (Time.length == 3) {
			return Time[0] + ':' + Time[1] + ':' + Time[2];
		}
		else
			return "Invalid format (Time)";
	}
	else
		return "Invalid format (Datetime)";
}

//---------------------------

/*
 * Metodos MAIN
 * Repuesta al guardar la tarea.
 * (Host -> Cliente)
 * OK - 20/12/02
*/
function rm_task_result(file_json) {
	let R = file_json["result"];
	if (R == 1)
		alert("Tarea guardada.")
	else
		alert("Error al guardar la tarea.");
}

/*
 * Metodos MAIN
 * Repuesta al remover una tarea.
 * (Host -> Cliente)
 * OK - 20/12/02
*/
function set_task_result(file_json) {
	let R = file_json["result"];
	if (R == 1)
		alert("Tarea removida.")
	else
		alert("Error al remover la tarea.");
}


/*
 * Metodos MAIN
 * Envia una tarea sea para actualizar o nueva (Host compara el nombre)
 * (Cliente -> Host)
 * VER - 20/12/02
 */
function set_task() {
	let data = { command: "set_task", a: "a", b: "b" }
	let json = JSON.stringify(data);
	connection.send(json);
}

/*
 * Metodos MAIN
 * Envia al host la terea a remover
 * (Cliente -> Host)
 * VER - 20/12/02
 */
function rm_task(name_task) {
	let data = { command: "rm_tasks", name_task: name_task }
	let json = JSON.stringify(data);
	connection.send(json);
}

/*
 * Metodos MAIN
 * Repuesta de actulizar fecha desde el servidor.
 * (Host -> Cliente)
 * OK - 20/11/20
*/
function result_std_status(file_json) {
	let R = file_json["result"];
	if (R == 1)
		alert("Dispocitivo activado.")
	else
		alert("Dispocitivo desactivado.");
}

/*
 * Metodos MAIN
 * Repuesta de actulizar fecha desde el servidor.
 * (Cliente -> Host)
 * OK - 20/11/20
*/
var on_off = 1;
function set_std_status() {
	if (on_off == 1)
		on_off = 0;
	else
		on_off = 1;

	let data = { command: "set_std_status", on_off: on_off }
	let json = JSON.stringify(data);
	connection.send(json);
}

/*
 * Metodos MAIN
 * Repuesta de actulizar fecha desde el servidor.
 * (Host -> Cliente)
 * OK - 20/11/16
*/
function result_clock(file_json) {
	let R = file_json["result"];
	if (R == 1)
		alert("Hora y fecha actualizada.");
	else
		alert("Error al actualizar la hora y fecha.")
}

/*
 * Metodos MAIN
 * Manda la fecha para actulizar en el servidor.
 * (Cliente -> Host)
 * OK - 20/11/16
 */
function set_clock() {
	let now = new Date();
	let now_date = now.getDate() + "/" + (now.getMonth() + 1) + "/" + now.getFullYear();
	let now_time = now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds();
	//alert("NOW: " + now + "\nDate: " + now_date + "\nTime: " + now_time);
	let data = { command: "set_clock", date: now_date, time: now_time }
	let json = JSON.stringify(data);
	connection.send(json);
}

/*
 * Metodos MAIN
 * Carga detalle de la tarea desde el servidor.
 * (Host -> Cliente)
 * VER - 20/11/14
 */
function load_task(file_json) {
	document.getElementById('sec_name').value = file_json["name"];
	document.getElementById('sec_iniDate').value = getDate(file_json["iniDate"]);
	document.getElementById('sec_iniTime').value = getTime(file_json["iniDate"]);
	document.getElementById('sec_endDate').value = getDate(file_json["endDate"]);
	document.getElementById('sec_endTime').value = getTime(file_json["endDate"]);

	alert(file_json["enable"]);
	document.getElementById('sec_enable').checked = true;//file_json["enable"] == "1" ? True : False;
	document.getElementById('sec_enable').checked = file_json["enable"];


	for (i = 1; i <= 4; i++) {

		let sec_color = "000000";
		let sec_index = 0;
		let sec_rotation = false;
		let sec_repeat = false;
		let sec_millis = 100;

		switch (i) {
			case 1:
				sec_color = StreamJson["sec1_rgb"];
				sec_index = StreamJson["sec1_index"];
				sec_rotation = StreamJson["sec1_rotation"];
				sec_repeat = StreamJson["sec1_repeat"];
				sec_millis = StreamJson["sec1_millis"];
				break;
			case 2:
				sec_color = StreamJson["sec2_rgb"];
				sec_index = StreamJson["sec2_index"];
				sec_rotation = StreamJson["sec2_rotation"];
				sec_repeat = StreamJson["sec2_repeat"];
				sec_millis = StreamJson["sec2_millis"];
				break;
			case 3:
				sec_color = StreamJson["sec3_rgb"];
				sec_index = StreamJson["sec3_index"];
				sec_rotation = StreamJson["sec3_rotation"];
				sec_repeat = StreamJson["sec3_repeat"];
				sec_millis = StreamJson["sec3_millis"];
				break;
			case 4:
				sec_color = StreamJson["sec4_rgb"];
				sec_index = StreamJson["sec4_index"];
				sec_rotation = StreamJson["sec4_rotation"];
				sec_repeat = StreamJson["sec4_repeat"];
				sec_millis = StreamJson["sec4_millis"];
				break;
			default:
				sec_index = 0;
				break;
		}

		switch (sec_index) {
			case 0:
				document.getElementById('sec' + i + '_option-0').checked = True;
				break;
			case 1:
				document.getElementById('sec' + i + '_option-1').checked = True;
				break;
			case 2:
				document.getElementById('sec' + i + '_option-2').checked = True;
				break;
			case 2:
				document.getElementById('sec' + i + '_option-3').checked = True;
				break;
			case 3:
				document.getElementById('sec' + i + '_option-4').checked = True;
				break;
			case 4:
				document.getElementById('sec' + i + '_option-5').checked = True;
				break;
			case 5:
				document.getElementById('sec' + i + '_option-6').checked = True;
				break;
		}

		document.getElementById('sec' + i + '_color').value = sec_color;
		document.getElementById('sec' + i + '_invert').checked = sec_rotation == 0 ? True : False;
		document.getElementById('sec' + i + '_repeat').value = sec_repeat;
		document.getElementById('sec' + i + '_time').value = sec_millis;
	}

	alert("END load_task");
}

/*
document.getElementById('sec_name').value = StreamJson.name;
document.getElementById('sec_iniDate').value = getDate(StreamJson.iniDate);
document.getElementById('sec_iniTime').value = getTime(StreamJson.iniDate);
document.getElementById('sec_endDate').value = getDate(StreamJson.endDate);
document.getElementById('sec_endTime').value = getTime(StreamJson.endDate);
document.getElementById('sec_enable').value = StreamJson.enable;
*/



/*
 * Metodos MAIN
 * Pide los detalles de un tarea al servidor.
 * (Cliente -> Host)
 * OK - 20/11/15
 */
function select_task(name_task) {
	//alert("Pide informacion de la tarea: " + name_task + " al servidor");
	let data = { command: "select_task", name: name_task }
	let json = JSON.stringify(data);
	connection.send(json);
}

/*
 * Metodos MAIN
 * Carga la informacion a la pagina [TITULO].
 * (Host -> Cliente)
 * OK - 20/11/14
 */
function title_main(file_json) {
	let flag_status = file_json['on_off'];
	let clock = file_json['clock'];
	let sec_run = file_json['run_sec'];
	let name_status = flag_status == 1 ? 'Encendido' : 'Apagado';
	let Text = "-> Estado: " + name_status + " | Ejecutando: " + sec_run + " | Reloj: " + clock;

	let element = document.getElementById("status");
	if (element) {
		padre = element.parentNode;
		padre.removeChild(element);
	}

	//	<span class="mdl-layout__title">Text</spam>
	let title = document.createElement("spam");
	title.classList.add("mdl-layout__title");
	title.setAttribute("id", "status");
	title.appendChild(document.createTextNode(Text));
	//<div class="mdl-layout-spacer" id="title_status">
	let title_status = document.getElementById("title_status");
	title_status.appendChild(title);
}

/*
 * Metodos MAIN
 * Carga la informacion a la pagina [HABILITA RGB].
 * (Host -> Cliente)
 * VER - 20/11/14
 */
function enableRGB_main(file_json) {
	let isRGB = file_json['isRGB'];
	// sacar el componente colorpiker de todas las secuencias
}

/*
 * Metodos MAIN
 * Carga la informacion a la pagina.
 * (Host -> Cliente)
 * OK - 20/11/14
 */
function load_main(file_json) {
	title_main(file_json);
	enableRGB_main(file_json);

	on_off = file_json['on_off'];
	let ntasks = file_json['n_tasks'];
	let tasks = file_json['tasks'];

	let menu = document.getElementById("menu");
	for (let i = 0; i < ntasks; i++) {

		let Items = tasks[i].split('|');
		//alert("0 - enable: " + Items[0]);
		//alert("1 - name: " + Items[1]);
		//alert("2 - inidate: " + Items[2]);
		//alert("3 - enddate: " + Items[3]);

		let element = document.getElementById("menu_item-" + i);
		if (element) {
			padre = element.parentNode;
			padre.removeChild(element);
		}

		//<li class="mdl-list__item mdl-list__item--two-line">
		let menu_item = document.createElement("li");
		menu_item.classList.add("mdl-list__item", "mdl-list__item--two-line");
		menu_item.setAttribute("id", "menu_item-" + i);

		//	<span class="mdl-list__item-primary-content">
		let detail = document.createElement("span");
		detail.classList.add("mdl-list__item-primary-content");

		//		<i class="material-icons mdl-list__item-avatar">schedule</i>
		let detail_icon = document.createElement("i");
		detail_icon.classList.add("material-icons", "mdl-list__item-avatar", "mdl_disable");
		detail_icon.appendChild(document.createTextNode("schedule"));

		//		<a href="#">TASK_NAME-1</a>
		let detail_title = document.createElement("a");
		detail_title.appendChild(document.createTextNode(Items[1]));
		detail_title.setAttribute("id", "detail_title-" + i);
		detail_title.setAttribute("href", "#");
		detail_title.onclick = function () { select_task(Items[1]); }
		//detail_title.onclick = function() { alert("Traer data: " + Items[1]); }
		//detail_title.onclick = function() { alert("Traer data: " + this.value); }


		//		<span class="mdl-list__item-sub-title">1/1/2020 00:00:00</span>
		let detail_ini = document.createElement("spam");
		detail_ini.classList.add("mdl-list__item-sub-title");
		detail_ini.appendChild(document.createTextNode("Inicio: " + Items[2]));

		//		<span class="mdl-list__item-sub-title">31/12/2020 23:59:59</span>
		let detail_end = document.createElement("spam");
		detail_end.classList.add("mdl-list__item-sub-title");
		detail_end.appendChild(document.createTextNode("Fin:" + Items[3]));


		detail.appendChild(detail_icon);
		detail.appendChild(detail_title);
		detail.appendChild(detail_ini);
		detail.appendChild(detail_end);
		menu_item.appendChild(detail);
		menu.appendChild(menu_item);

	}
}

/*
 * Metodos MAIN
 * Solicita informacion al Host.
 * (Cliente -> Host)
 * OK - 20/11/14
 */
function main_onload() {
	let data = { command: "main_onload" }
	let json = JSON.stringify(data);
	connection.send(json);
}

/*
 * Metodos AP
 * Guarda configuracion.
 * (Cliente -> Host)
 * OK - 20/11/14
 */
function save_ssid() {
	let ssid = document.getElementById("ssid").value;
	let pwd = document.getElementById("pwd").value;
	let data = { command: "write_wifi", ssid: ssid, password: pwd }
	let json = JSON.stringify(data);
	connection.send(json);
}

/*
 * Metodos AP
 * Carga la tabla a mostrar.
 * (Host -> Cliente)
 * NO USADO - VER - 20/11/13
 */
function reload_table(file_json) {

	let scanWifi = file_json['networks'];

	let title = document.getElementById("title_wifi");
	let n_detetec = scanWifi['n_detected_networks'];
	title.value = 'Wi-Fi networks detected: ' + n_detetec;
	//title.textContent(scanWifi['n_detected_networks']);

	element = document.getElementById("dtSSID");
	if (element) {
		padre = element.parentNode;
		padre.removeChild(element);
	}

	let body = document.getElementById("dtSSID");
	let tabla = document.createElement("table");
	let tblHead = document.createElement("thead");
	let tblBody = document.createElement("tbody");

	tabla.classList.add("mdl-data-table", "mdl-js-data-table");

	let trHead = document.createElement("tr");

	let tdN = document.createElement("td");
	tdN.appendChild(document.createTextNode("#"));
	trHead.appendChild(tdN);

	let tdSSID = document.createElement("td");
	tdSSID.appendChild(document.createTextNode("SSID"));
	trHead.appendChild(tdSSID);

	let tdRSSI = document.createElement("td");
	tdRSSI.appendChild(document.createTextNode("RSSI"));
	trHead.appendChild(tdRSSI);

	let tdENCR = document.createElement("td");
	tdENCR.appendChild(document.createTextNode("Encryption"));
	trHead.appendChild(tdENCR);

	tblHead.appendChild(trHead);

	// Crea las filas [tbody]
	for (let nRow = 1; nRow <= scanWifi.Length; nRow++) {
		let row = document.createElement("tr");

		let tdContentN = document.createElement("td");
		tdContentN.classList.add("mdl-data-table__cell--non-numeric");
		tdContentN.appendChild(document.createTextNode(nRow));
		row.appendChild(tdContentN);

		let tdContentSSID = document.createElement("td");
		tdContentSSID.classList.add("mdl-data-table__cell--non-numeric");
		tdContentSSID.appendChild(document.createTextNode(scanWifi[nRow].SSID));
		row.appendChild(tdContentSSID);

		let tdContentRSSI = document.createElement("td");
		tdContentRSSI.classList.add("mdl-data-table__cell--non-numeric");
		tdContentRSSI.appendChild(document.createTextNode(scanWifi[nRow].RSSI));
		row.appendChild(tdContentRSSI);

		let tdContentENCR = document.createElement("td");
		tdContentENCR.classList.add("mdl-data-table__cell--non-numeric");
		tdContentENCR.appendChild(document.createTextNode(scanWifi[nRow].ENC));
		row.appendChild(tdContentENCR);

		tblBody.appendChild(row);
	}
	tabla.appendChild(tblHead);
	tabla.appendChild(tblBody);
	body.appendChild(tabla);
}

/*
 * Metodos AP
 * Solicita scaneo de redes.
 * (Cliente -> Host)
 * NO USADO - OK - 20/11/13
 */
function refresh_table_wifi() {
	let data = { command: "scan_wifi" }
	let json = JSON.stringify(data);
	connection.send(json);
}

//-------------------------------POR REVISAR

function ocultar() {
	document.getElementById('detail_task').style.display = 'none';
}

function mostrar() {
	document.getElementById('detail_task').style.display = 'block';
}

var test = 1;
function set_test() {
	if (test == 1)
		test = 0;
	else
		test = 1;

	alert("set_test:\ntest= " + test);

	let data = { command: "set_test", test: test }
	let json = JSON.stringify(data);
	connection.send(json);
}

//---------------------------

function writeTask(StreamJson) {
	document.getElementById('sec_name').value = StreamJson.name;
	document.getElementById('sec_iniDate').value = getDate(StreamJson.iniDate);
	document.getElementById('sec_iniTime').value = getTime(StreamJson.iniDate);
	document.getElementById('sec_endDate').value = getDate(StreamJson.endDate);
	document.getElementById('sec_endTime').value = getTime(StreamJson.endDate);
	document.getElementById('sec_enable').checked = StreamJson.enable == 1 ? True : False;

	for (i = 1; i <= 4; i++) {

		var sec_color = 0;
		var sec_index = 0;
		var sec_rotation = False;
		var sec_repeat = False;
		var sec_millis = 100;

		switch (i) {
			case 1:
				sec_color = StreamJson.sec1_rgb;
				sec_index = StreamJson.sec1_index;
				sec_rotation = StreamJson.sec1_rotation;
				sec_repeat = StreamJson.sec1_repeat;
				sec_millis = StreamJson.sec1_millis;
				break;
			case 2:
				sec_color = StreamJson.sec2_rgb;
				sec_index = StreamJson.sec2_index;
				sec_rotation = StreamJson.sec2_rotation;
				sec_repeat = StreamJson.sec2_repeat;
				sec_millis = StreamJson.sec2_millis;
				break;
			case 3:
				sec_color = StreamJson.sec3_rgb;
				sec_index = StreamJson.sec3_index;
				sec_rotation = StreamJson.sec3_rotation;
				sec_repeat = StreamJson.sec3_repeat;
				sec_millis = StreamJson.sec3_millis;
				break;
			case 4:
				sec_color = StreamJson.sec4_rgb;
				sec_index = StreamJson.sec4_index;
				sec_rotation = StreamJson.sec4_rotation;
				sec_repeat = StreamJson.sec4_repeat;
				sec_millis = StreamJson.sec4_millis;
				break;
			default:
				sec_index = 0;
				break;
		}

		switch (sec_index) {
			case 0:
				document.getElementById('sec' + i + '_option-0').checked = True;
				break;
			case 1:
				document.getElementById('sec' + i + '_option-1').checked = True;
				break;
			case 2:
				document.getElementById('sec' + i + '_option-2').checked = True;
				break;
			case 2:
				document.getElementById('sec' + i + '_option-3').checked = True;
				break;
			case 3:
				document.getElementById('sec' + i + '_option-4').checked = True;
				break;
			case 4:
				document.getElementById('sec' + i + '_option-5').checked = True;
				break;
			case 5:
				document.getElementById('sec' + i + '_option-6').checked = True;
				break;
		}

		document.getElementById('sec' + i + '_color').value = sec_color;
		document.getElementById('sec' + i + '_invert').checked = sec_rotation == 0 ? True : False;
		document.getElementById('sec' + i + '_repeat').value = sec_repeat;
		document.getElementById('sec' + i + '_time').value = sec_millis;
	}
}


function updateGPIO(id, status) {
	document.getElementById('input-label-' + id).textContent = status;

	if (status == 'ON') {
		document.getElementById('input-label-' + id).classList.add('On-style');
		document.getElementById('input-label-' + id).classList.remove('Off-style');
	}
	else {
		document.getElementById('input-label-' + id).classList.add('Off-style');
		document.getElementById('input-label-' + id).classList.remove('On-style');
	}
}