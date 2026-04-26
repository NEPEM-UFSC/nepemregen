# Script para execução do NEPEM REGEN no QEMU (ESP32 Emulation)

# Requisitos:
# 1. ESP-IDF instalado e configurado
# 2. QEMU para ESP32 (disponível via idf-tools ou fork da Espressif)

Write-Host "Iniciando processo de Emulação via QEMU..." -ForegroundColor Cyan

# 1. Build para o target real (ESP32)
# idf.py build

# 2. Localização do binário e bootloader
$BUILD_DIR = "build"
$BIN_FILE = "$BUILD_DIR/edge_appliance.bin"

if (-Not (Test-Path $BIN_FILE)) {
    Write-Host "Erro: Binário não encontrado em $BIN_FILE. Certifique-se de rodar 'idf.py build' primeiro." -ForegroundColor Red
    exit 1
}

# 3. Comando QEMU (Exemplo de uso comum)
# qemu-system-xtensa -nographic -machine esp32 -drive file=$BUILD_DIR/flash_image.bin,if=mtd,format=raw

Write-Host "Para rodar no QEMU, você precisa gerar a imagem da flash combinada ou passar os argumentos de carregamento."
Write-Host "Use o comando abaixo se o QEMU estiver no seu PATH:" -ForegroundColor Yellow
Write-Host "qemu-system-xtensa -nographic -machine esp32 -drive file=build/flash_image.bin,if=mtd,format=raw"

# Nota: O ESP-IDF v5+ possui integração direta:
# idf.py qemu
