square="square"
triangle="triangle"

var="square"

echo " _________________"

for i in $(seq 1 3); do
	if true; #"$var" = "$square"
	then
		echo "|  ___       ___  |"
		echo "| |_|_|     |_|_| |"
		echo "| |_|_|     |_|_| |"
	elif false #"$var" = "$triangle"
	then
		echo "|                 |"
		echo "|   /\\       /\\   |"
		echo "|  /__\\     /__\\  |"
	fi
	echo "|                 |"
done


echo "|        _        |"
echo "|       | |       |"
echo "|       | |       |"
echo "|_______| |_______|"
