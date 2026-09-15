import { useState } from "react";
import { invoke } from "@tauri-apps/api/core";
import { open } from '@tauri-apps/plugin-dialog';
import { Button } from "./components/ui/button";

function App() {
  const [playError, setPlayError] = useState("");
  const [romPath, setRomPath] = useState("");
  const [boothPath, setBoothPath] = useState("");

  async function play() {
    setPlayError("");
    try {
      console.log("Playing game with paths:", romPath, boothPath);
      await invoke("play_game", {
        romPath,
        boothPath,
      });
    } catch (e) {
      setPlayError(String(e));
    }
  }

  return (
    <main className="container">
    <div className="flex">       <img className="max-w-64" src="https://assets-prd.ignimgs.com/2021/12/14/zeldaoracleofseasons-1639524385813.jpg" />
<div className="grow"> <div className="grow flex gap-1"><Button className={"flex grow"} onClick={async () => {
        const file = await open({
          multiple: false,
          directory: false,
        });
    setRomPath(file as string);
      }}>Add your rom</Button>
      <Button  className={"flex grow"} onClick={async () => {
        const file = await open({
          multiple: false,
          directory: false,
        });
        setBoothPath(file as string);
      }}>Add your bin</Button>
        </div>

        <Button type="button" className={"w-full"} onClick={play} disabled={!romPath || !boothPath}>
          Play
        </Button>
      {playError && <p style={{ color: "red" }}>{playError}</p>}</div>
    </div>
    </main>
  );
}

export default App;
