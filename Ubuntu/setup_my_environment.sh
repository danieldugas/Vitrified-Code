sudo echo "------- SETTING UP BASHRC ------------"
# bashrc
ln -s ~/Documents/Vitrified-Code/Ubuntu/bashrc ~/.bashrc_ext
if [ -z "$MACHINE_NAME" ]; then
  echo "Name of this machine: "
  read machine_name
  echo "# Added by setup_my_environment.sh script (Daniel)" >> ~/.bashrc
  echo "export MACHINE_NAME=$machine_name" >> ~/.bashrc
  echo "source ~/.bashrc_ext" >> ~/.bashrc
fi

echo "------- SETTING UP GIT ------------"
sudo apt install git
# git config
git config --global user.email "exodaniel@gmail.com"
git config --global push.default simple

echo "------- CLONING PYNIEL ------------"
# git clone git@github.com:danieldugas/Vitrified-Code.git
git clone https://github.com/danieldugas/pyniel.git ~/Documents/pyniel

echo "------- SETTING UP GIT-WATCH ------------"
git clone https://github.com/danieldugas/git-watch.git ~/Documents/git-watch
echo "~/Documents/Vitrified-Code" >> ~/git_repos_to_watch.txt
echo "~/Documents/pyniel" >> ~/git_repos_to_watch.txt

echo "------- SETTING UP VIM ------------"
sudo apt install vim-gtk3 -y
ln -s ~/Documents/Vitrified-Code/Vim/vimrc ~/.vimrc
mkdir -p ~/.vim/colors
cp ~/Documents/Vitrified-Code/Vim/twilight.vim ~/.vim/colors/
git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim

echo "------- SETTING UP HSTR ------------"
# better ctrl-r search
sudo add-apt-repository ppa:ultradvorka/ppa -y
sudo apt-get update
sudo apt-get install hstr -y

echo "------- SETTING UP UMEDIT ------------"
ln -s ~/Documents/Vitrified-Code/Ubuntu/man ~/man
ln -s ~/Documents/Vitrified-Code/Ubuntu/templates ~/templates

echo "------- SETTING UP UBUNTU GUI PREFERENCES ------------"
## Ubuntu GUI
# Load gnome terminal profile (replace load with dump and < with > to save)
dconf load /org/gnome/terminal/ < ~/Documents/Vitrified-Code/Ubuntu/gnome-terminal-profiles.txt
# compiz config settings manager
sudo apt install compizconfig-settings-manager python-compizconfig -y
python ~/Documents/Vitrified-Code/Ubuntu/import_compiz_settings.py ~/Documents/Vitrified-Code/Ubuntu/compiz.profile
# Enable workspaces
gsettings set org.compiz.core:/org/compiz/profiles/unity/plugins/core/ hsize 2
gsettings set org.compiz.core:/org/compiz/profiles/unity/plugins/core/ vsize 2
# Gnome Extensions (TODO)
# Keybindings
# gsettings set org.gnome.desktop.wm.keybindings move-to-monitor-left "['<Primary><Super>h']"
# gsettings set org.gnome.desktop.wm.keybindings move-to-monitor-left "['<Primary><Super>;']"
# Behavior
# gsettings set org.gnome.shell.overrides workspaces-only-on-primary false
# Oneclick extract
sudo apt install p7zip-full
sudo cp ~/Documents/Vitrified-Code/Ubuntu/oneclickextract.desktop /usr/share/applications/
