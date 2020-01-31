package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"os"
	"os/exec"
	"runtime"
	"strconv"

	"../mmcompress"
)

const USAGE = `mmcompress_wui [OPTIONS]
  -h/--help: Print this help.
  -p/--port PORT: Port to use.
`

func main() {

	var err error
	port := 0

	i, nbArgs := 1, len(os.Args)
	for i < nbArgs {
		arg := os.Args[i]
		if arg == "-h" || arg == "--help" {
			fmt.Printf(USAGE)
			os.Exit(0)
		} else if arg == "-p" || arg == "--port" {
			i++
			port, err = strconv.Atoi(os.Args[i])
			if err != nil {
				log.Fatalf("Could not convert to port: %s", os.Args[i])
			}
		} else {
			fmt.Printf("%s\n", USAGE)
			log.Fatalf("Unknown arg: %s\n", arg)
		}
		i++
	}

	listener, err := net.Listen("tcp", fmt.Sprintf(":%d", port))
	if err != nil {
		panic(err)
	}
	port = listener.Addr().(*net.TCPAddr).Port
	fmt.Println("Using port:", port)

	http.HandleFunc("/check", serveCheck)
	http.HandleFunc("/run", serveRun)
	http.HandleFunc("/", serveRoot)

	err = openWebBrowser(fmt.Sprintf("http://localhost:%d", port))
	if err != nil {
		log.Println(err)
	}

	panic(http.Serve(listener, nil))
}

func openWebBrowser(url string) error {
	var cmd string
	var args []string

	switch runtime.GOOS {
	case "windows":
		cmd = "cmd"
		args = []string{"/c", "start"}
	case "darwin":
		cmd = "open"
	default: // "linux", "freebsd", "openbsd", "netbsd"
		cmd = "xdg-open"
	}
	args = append(args, url)
	return exec.Command(cmd, args...).Start()
}

func serveRoot(w http.ResponseWriter, req *http.Request) {
	fmt.Fprint(w, `
		<html>
			<head>
				<title>mmcompress</title>
				<style>
					#warnings {
						color: red;
					}
					.marg {
						margin: .5em;
					}
				</style>
			<head>
			<body>
				<h1>mmcompress</h1>
				<p id="warnings"></p>
				<div id="inputs">
					<div class="marg">Input: <input type="file" id="input" webkitdirectory></div>
					<div class="marg">Output: 
						<select id="output-mode">
							<option value="copy" selected>Copy</option>
							<option value="overwrite">Overwrite</option>
						</select>
						<input type="file" id="output">
					</div>
					<div class="marg">Compression level:
						<select id="compression-level">
							<option value="S">SMALL</option>
							<option value="M" selected>MEDIUM</option>
							<option value="L">LARGE</option>
						</select>
					</div>
					<div>
						<button id="toggle-details">Technical details</button>
						<div id="details" style="display:none">
							<div class="marg">
								<input type="number" id="img-min-length" placeholder="Image min length">
								<input type="number" id="vid-bitrate" placeholder="Video bitrate">
								<input type="number" id="vid-crf" placeholder="Video CRF">
							</div>
							<div class="marg">Tmp dir: <input type="file" id="tmp-dir"></div>
						</div>
					</div>
					<div><button id="run">GO</button></div>
				</div>
				<p><textarea style="width:100%; min-height:10em"></textarea></div>

				<script>
					window.onload = () => {
						check()

						function syncOuput(){
							const outputMode = getSelectVal(Qid("output-mode"))
							Qid("output").style.display = (outputMode === "overwrite") ? "none" : null
						}
						syncOuput()
						Qid("output-mode").onchange = syncOuput

						Qid("toggle-details").onclick = toggleDetails

						Qid("run").onclick = run

						Qid("input").onchange = evt => console.log(evt.target.files[0].webkitRelativePath)
					}
					async function check(){
						const check = await xhrGet("/check")
						let warnings = ""
						if(!check.imgCompressor)
							warnings += "<div>You should install Image Magick</div>"
						if(!check.vidCompressor)
							warnings += "<div>You should install FFMpeg</div>"
						Qid("warnings").innerHTML = warnings
					}
					function toggleDetails(){
						const detailsStyle = Qid("details").style
						detailsStyle.display = detailsStyle.display ? null : "none"
					}
					async function run(){
						xhrPost("/run", {
							input: getInputVal(Qid("input")),
							outputMode: getSelectVal(Qid("output-mode")),
							output: getInputVal(Qid("output")),
							compressionLevel: getSelectVal(Qid("compression-level")),
							imgMinLength: getInputVal(Qid("compression-level")),
							vidBitrate: getInputVal(Qid("vid-bitrate")),
							vidCrf: getInputVal(Qid("vid-crf")),
						})
					}

					// utils

					function Qid(id){ return document.getElementById(id) }
					function getSelectVal(el){
						return el.options[el.selectedIndex].value
					}
					function getInputVal(el){
						const val = el.value
						return val ? val : null
					}
					async function xhrGet(url){
						const res = await fetch(url)
						return await res.json()
					}
					async function xhrPost(url, data){
						const res = await fetch(url, {
							method: 'POST',
							headers: { 'Content-Type': 'application/json' },
							body: JSON.stringify(data)
						})
					}

				</script>
			</body>
		</html>
	`)
}

func serveCheck(w http.ResponseWriter, req *http.Request) {
	ctx := mmcompress.NewContext()
	imgCompressor := mmcompress.GetImgCompressor(ctx)
	vidCompressor := mmcompress.GetVideoCompressor(ctx)
	data := map[string]bool{
		"imgCompressor": (imgCompressor != ""),
		"vidCompressor": (vidCompressor != ""),
	}
	replyJson(w, data)
}

func serveRun(w http.ResponseWriter, req *http.Request) {
	body, err := ioutil.ReadAll(req.Body)
	if err != nil {
		replyErr(w, err)
		return
	}
	ctx := mmcompress.NewContext()
	json.Unmarshal(body, &ctx)
	err = mmcompress.MMC(ctx)
	if err != nil {
		replyErr(w, err)
		return
	}
	replyOK(w)
}

func replyOK(w http.ResponseWriter) {
	w.WriteHeader(200)
}

func replyJson(w http.ResponseWriter, data interface{}) {
	js, err := json.Marshal(data)
	if err != nil {
		replyErr(w, err)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	w.Write(js)
}

func replyErr(w http.ResponseWriter, err error) {
	http.Error(w, err.Error(), http.StatusInternalServerError)
}
