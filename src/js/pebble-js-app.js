var MSG = "   Hello World!  " + "\n";
var SUNSHINE = true;

// Function to send a message to the Pebble using AppMessage API
function send_message(status, msg) {
	Pebble.sendAppMessage({'status': status, 'message': msg});
}

// Called when JS is ready
Pebble.addEventListener("ready", function(e) {
	MSG += getTime();
	send_message(SUNSHINE ? 1 : 0, MSG);
});

function getTime() {
	var d = new Date();
	var H = d.getHours();
	if (H > 12) {
		H = H - 12;
		SUNSHINE = true;
	} else SUNSHINE = false;
	if (H <= 9) H = '0' + H;
	var M = d.getMinutes();
	if (M <= 9) M = '0' + M;
	var S = d.getSeconds();
	if (S <= 9) S = '0' + S;
	return "---->" + H + ":" + M + "<----" + "\n";
}

// Called when incoming message from the Pebble is received
Pebble.addEventListener("appmessage", function(e) {
	console.log("Received Status: " + e.payload.status);
	console.log("Received Message: " + e.payload.message);
		
	MSG = "   Hello World!  " + "\n" + 
		getTime() + 
		"";
	
	if (e.payload.status == 3) {
		MSG += "        Timer!" + "\n";
	} else MSG += e.payload.message + (e.payload.status > 0 ? " pressed" : " released");
	send_message(SUNSHINE ? 1 : 0, MSG);
});