# Scripts Directory

This directory contains utility scripts for working with Astro.

## Configuration

The scripts use a JSON configuration file (`config.json`) to manage paths and parameters. The configuration includes:

- Model paths for VAD, KWS, and ASR models
- Default parameters for keyword generation
- Build directory location

You can modify the config file to change default paths and parameters. The config file is located in the scripts directory.

## Keyword Generation Script

The `generate_keywords.sh` script helps generate keywords files for use with sherpa-onnx keyword spotting using the official `sherpa-onnx-cli` tool.

### Prerequisites

1. sherpa-onnx Python package:
   ```bash
   pip install sherpa-onnx
   ```

2. jq command-line tool:
   ```bash
   # macOS
   brew install jq
   
   # Ubuntu/Debian
   sudo apt-get install jq
   
   # CentOS/RHEL
   sudo yum install jq
   ```

### Usage

```bash
./generate_keywords.sh --keywords "keyword1" "keyword2" "keyword3" \
                      [--output <output-file>] \
                      [--threshold <value>] \
                      [--tokens-type <type>] \
                      [--config <config-file>]
```

### Arguments

- `--keywords`: One or more keywords to generate files for (required)
- `--output`: Path to output keywords file (optional, defaults to value from config)
- `--threshold`: Detection threshold (optional, defaults to value from config)
- `--tokens-type`: Type of tokens (optional, defaults to value from config)
- `--config`: Path to config file (optional, defaults to scripts/config.json)

### Example

```bash
./generate_keywords.sh --keywords "hey astro" "computer"
```

This will create a keywords file in the default output directory with the specified keywords, using the default parameters from the config file.

The script uses `sherpa-onnx-cli text2token` to convert the keywords into the proper token format. Each keyword is assigned the default boosting score and threshold value from the config file. 