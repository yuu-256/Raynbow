#!/bin/zsh

cat_spinner() {
  local pid=$1
  local delay=0.05
  local frames=(
    "(=^･ω･^=) "
    "( =^･ω･^=)"
    "(  =^･ω･^=)"
    "(   =^･ω･^=)"
    "(    =^･ω･^=)"
    "(     =^･ω･^=)"
    "(      =^･ω･^=)"
    "(     =^･ω･^=)"
    "(    =^･ω･^=)"
    "(   =^･ω･^=)"
    "(  =^･ω･^=)"
    "( =^･ω･^=)"
  )
  local n=${#frames[@]}
  local i=0

  while kill -0 "$pid" 2>/dev/null; do
    local t=$(echo "$i / $n * 6.28" | bc -l)  # 0〜2π
    local r=$(printf "%.0f" "$(echo "s($t) * 127 + 128" | bc -l)")
    local g=$(printf "%.0f" "$(echo "s($t + 2.094) * 127 + 128" | bc -l)")
    local b=$(printf "%.0f" "$(echo "s($t + 4.188) * 127 + 128" | bc -l)")

    printf "\r\033[2K\033[38;2;%d;%d;%dm%s\033[0m" "$r" "$g" "$b" "${frames[i]}"

    i=$(((i + 1) % n))
    sleep $delay
  done

  printf "\r\033[0K"
}

# as default
diameter=""
lambda_min=""
lambda_max=""

echo "Cleaning up previous outputs..."
echo " "
rm -f output/*(N)

# Print the banner
./interface/print_banner

# Interaction with user
echo " "
echo "Set the diameter of the water drop (nm):"
echo "粒子の直径を設定してね (nm):"
read diameter
echo " "
echo "Set the minimum wavelength of light (nm):"
echo "光の最小波長を設定してね (nm):"
read lambda_min
echo " "
echo "Set the maximum wavelength of light (nm):"
echo "光の最大波長を設定してね (nm):"
read lambda_max

echo " "
echo "Thanks."
echo "ありがとう！"
echo " "
echo "Calculating the scattering phase function from Mie scattering theory..."
echo "Mie散乱理論から散乱位相関数を計算しているよ......"
echo " "
echo "diameter: $diameter nm"
echo "lambda_min: $lambda_min nm"
echo "lambda_max: $lambda_max nm"
echo " "

# Create the input file
python3 utils/create_input_mie.py "$diameter"

# Run the Mie scattering calculation
cd LX-MIE && ./lx_mie input/parameters_phase_function_2000.input & cat_spinner $!
cd ..

echo "Processing ray tracing......"
echo "光線追跡を処理中だよ......"
echo " "

# Run the ray tracing calculation
./simulate LX-MIE/output/phase_function.dat "$lambda_min" "$lambda_max" & cat_spinner $!

echo "Rendering the rainbow image......"
echo "虹の画像をレンダリング中だよ......"
echo " "

# Run the visualization script
python3 utils/visualize_rainbow.py & cat_spinner $!

echo "All done!"
echo "終わったみたいだよ！"

sleep 2
open output/rainbow.png
