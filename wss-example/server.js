const WebSocket = require('ws')
const http = require("http")
let iotClient = null
let frontendClient = null

const app = http.createServer((req, res) => {
    const method = req.method
    const url = req.url
    if (method === 'GET') {
        if (url === '/auto') {
            if (iotClient && iotClient.readyState === WebSocket.OPEN) iotClient.send("auto")
            res.end("OK")
        } else if (url === '/manual') {
            if (iotClient && iotClient.readyState === WebSocket.OPEN) iotClient.send("manual")
            res.end("OK")
        }
    }
})

const wss = new WebSocket.Server({ server: app, clientTracking: true })

wss.on("connection", (socket, req) => {
    const time = new Date(Date.now()).toLocaleTimeString()
    /** 
     * Create specific communication from one identified socket to all socket using
     * setAuthorization() method provided in arduino websocket library. 
        if (req.headers.authorization) {
            const encodedAuth = req.headers.authorization.replace('Basic ', '').trim() ?? null
            const decodedString = Buffer.from(encodedAuth, 'base64').toString('utf-8')
                const [username, password] = decodedString.split(':')
                if ((username === "esp32" && password === "himom!") && (iotClient && frontendClient)) {
                    frontendClient.send("")
                }
            }
     */
    
    const IPAddress = req.socket.remoteAddress

    if (socket._protocol === 'arduino') {
        iotClient = socket
        iotClient.send(`IoT identified:${IPAddress}`)
        iotClient.onopen = () => {
            frontendClient.send(`[info]:${time}:iot_online`)
        }
        iotClient.onclose = () => {
            frontendClient.send(`[info]:${time}:iot_offline`)
        }
        console.log(`iot device found:${IPAddress}`)
    } else {
        frontendClient = socket
        frontendClient.send(`Frontend App identified:${IPAddress}`)
        console.log(`frontend app found:${IPAddress}`)
    }
    
    console.log(`connected clients: ${wss.clients.size}`)
    
    iotClient && iotClient.on("message", (data) => {
        console.log(`${time}::[IoT]message: ${data}`)
    })
    frontendClient && frontendClient.on("message", (data) => {
        console.log(`${time}::[frontend]message: ${data}`)
    })

    /** this line do the same thing, handling message event
     * socket.addEventListener("message", ({ data }) => console.log(data.toString()))
     * socket.addListener("message", ({ data }) => console.log(data.toString()))
     * socket.onmessage = ({ data }) => console.log(data.toString())
     * socket.on("message", (data) => console.log(data.toString()))
     */

    if (frontendClient && iotClient) {
        // const time = new Date(Date.now()).toLocaleTimeString()
        iotClient.onmessage = ({ data }) => {
            frontendClient.send(data)
        }
    }
})

app.listen(80, '0.0.0.0', () => console.log('server is running'))
