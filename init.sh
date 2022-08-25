echo "[git pull]"
git pull

echo "[Reload Project]"
cmake -B ~/rtcamp2022/build -DCMAKE_BUILD_TYPE=Release

echo "[Build]"
cmake --build ~/rtcamp2022/build --config Release