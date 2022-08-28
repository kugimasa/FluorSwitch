echo "[git pull]"
git pull

echo "[Reload Project]"
cmake -B ~/FluorSwitch/build -DCMAKE_BUILD_TYPE=Release

echo "[Build]"
cmake --build ~/FluorSwitch/build --config Release