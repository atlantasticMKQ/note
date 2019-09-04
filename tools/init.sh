mkdir ~/bin
cp ./note-sync-init ~/bin
mkdir ~/.ssh
cp ./config ~/.ssh
cp ./mkq ~/.ssh
echo "bash ~/bin/note-sync-init" >> ~/.bashrc
source ~/.bashrc
