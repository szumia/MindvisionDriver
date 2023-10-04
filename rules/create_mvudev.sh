sudo cp ./mvusb.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules 
sudo udevadm trigger
echo "finish"
