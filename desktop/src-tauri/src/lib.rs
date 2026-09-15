use serde_json::de;



#[tauri::command]
fn play_game(rom_path: &str, booth_path: &str) -> Result<(), String> {
    use std::path::PathBuf;
    use std::process::Command;

    let repo_root = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("../..")
        .canonicalize()
        .map_err(|e| format!("cannot resolve repo root: {e}"))?;

    let exe = repo_root.join("build-sdl/oracles");
    let rom = PathBuf::from(rom_path);
    let boot = PathBuf::from(booth_path);


    if !exe.exists() {
        return Err(format!(
            "emulator binary not found at {}; build it with: cmake -B build-sdl -DORACLES_SDL=ON && cmake --build build-sdl --target oracles",
            exe.display()
        ));
    }
    if !rom.exists() {
        return Err(format!("ROM not found at {}", rom.display()));
    }

    Command::new(exe)
        .arg(rom)
        .arg(boot)
        .spawn()
        .map_err(|e| format!("failed to launch emulator: {e}"))?;

    Ok(())
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_dialog::init())
        .invoke_handler(tauri::generate_handler![play_game])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
