#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
CONFIG_FILE="$SCRIPT_DIR/config.json"

# Function to get JSON value
get_json_value() {
    local key="$1"
    jq -r "$key" "$CONFIG_FILE"
}

# Check if jq is installed
if ! command -v jq &> /dev/null; then
    echo "Error: jq is required but not installed. Please install it:"
    echo "  macOS: brew install jq"
    echo "  Ubuntu/Debian: sudo apt-get install jq"
    echo "  CentOS/RHEL: sudo yum install jq"
    exit 1
fi

# Check if sherpa-onnx-cli is installed
if ! command -v sherpa-onnx-cli &> /dev/null; then
    echo "Error: sherpa-onnx-cli not found. Please install sherpa-onnx:"
    echo "pip install sherpa-onnx"
    exit 1
fi

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --keywords)
            shift
            KEYWORDS=()
            while [[ $# -gt 0 && ! "$1" =~ ^-- ]]; do
                KEYWORDS+=("$1")
                shift
            done
            ;;
        --output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        --threshold)
            THRESHOLD="$2"
            shift 2
            ;;
        --tokens-type)
            TOKENS_TYPE="$2"
            shift 2
            ;;
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Check if config file exists
if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Config file not found: $CONFIG_FILE"
    exit 1
fi

# Get default values from config
MODEL_PATH=$(get_json_value '.models.kws.path')
DEFAULT_THRESHOLD=$(get_json_value '.keywords.default_threshold')
DEFAULT_BOOST=$(get_json_value '.keywords.default_boost')
DEFAULT_TOKENS_TYPE=$(get_json_value '.models.kws.tokens_type')
DEFAULT_OUTPUT_DIR=$(get_json_value '.keywords.output_dir')

# Check required arguments
if [ -z "$KEYWORDS" ]; then
    echo "Usage: $0 --keywords <keyword1> [keyword2 ...] [--output <output-file>] [--threshold <value>] [--tokens-type <type>] [--config <config-file>]"
    echo "  --keywords: One or more keywords to generate files for"
    echo "  --output: Path to output keywords file (default: $DEFAULT_OUTPUT_DIR/keywords.txt)"
    echo "  --threshold: Detection threshold (default: $DEFAULT_THRESHOLD)"
    echo "  --tokens-type: Type of tokens (default: $DEFAULT_TOKENS_TYPE)"
    echo "  --config: Path to config file (default: $CONFIG_FILE)"
    exit 1
fi

# Set default values
THRESHOLD=${THRESHOLD:-$DEFAULT_THRESHOLD}
TOKENS_TYPE=${TOKENS_TYPE:-$DEFAULT_TOKENS_TYPE}
OUTPUT_FILE=${OUTPUT_FILE:-"$DEFAULT_OUTPUT_DIR/keywords.txt"}

# Create temporary input file
TEMP_INPUT=$(mktemp)
for keyword in "${KEYWORDS[@]}"; do
    echo "$keyword :$DEFAULT_BOOST #$THRESHOLD" >> "$TEMP_INPUT"
done

# Create output directory if it doesn't exist
mkdir -p "$(dirname "$OUTPUT_FILE")"

# Generate tokens using sherpa-onnx-cli
sherpa-onnx-cli text2token \
    --text "$TEMP_INPUT" \
    --tokens "$MODEL_PATH/tokens.txt" \
    --tokens-type "$TOKENS_TYPE" \
    --bpe-model "$MODEL_PATH/bpe.model" \
    "$OUTPUT_FILE"

# Clean up
rm "$TEMP_INPUT"

echo "Keywords file generated: $OUTPUT_FILE" 