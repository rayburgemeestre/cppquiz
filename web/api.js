class API {
    constructor(endpoint, unique_id, on_status_change, on_message, on_connected, on_disconnected) {
        this.endpoint = endpoint;
        this.unique_id = unique_id;
        this.on_status_change = on_status_change;
        this.on_message = on_message;
        this.on_connected = on_connected;
        this.on_disconnected = on_disconnected;
        this.ws = false;
        this.retry = false;

        this.connect();
    }

    connect() {
        this.on_status_change('connecting');
        let protocol = document.location.protocol.replace('http', 'ws');
        if (document.location.href.indexOf('localhost')) {
            this.ws = new WebSocket(protocol + '//' + document.location.host.replace(':8080', ':18080') + '/' + this.endpoint);
        } else {
            this.ws = new WebSocket(protocol + '//' + document.location.host + '/' + this.endpoint);
        }
        this.ws.onopen = function () {
            clearTimeout(this.retry);
            this.on_status_change('connected');
            this.on_connected();
        }.bind(this);
        this.ws.onclose = function () {
            this.on_status_change('disconnected');
            this.on_disconnected();
            this.retry = setTimeout(this.connect.bind(this), 1000);
        }.bind(this);
        this.ws.onmessage = function (message) {
            this.on_message(JSON.parse(message.data));
        }.bind(this);
        this.ws.onerror = function (error) {
            this.on_status_change("ERROR: " + error);
        }.bind(this);
    }

    send(msg) {
        msg['unique_id'] = this.unique_id;
        this.ws.send(JSON.stringify(msg));
    }
}

function uuidv4() {
    return ([1e7]+-1e3+-4e3+-8e3+-1e11).replace(/[018]/g, c =>
        (c ^ crypto.getRandomValues(new Uint8Array(1))[0] & 15 >> c / 4).toString(16)
    );
}