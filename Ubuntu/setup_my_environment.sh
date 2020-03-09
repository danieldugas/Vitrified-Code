sudo echo "------- SETTING UP BASHRC ------------"
# bashrc
ln -s ~/Code/Vitrified-Code/Ubuntu/bashrc ~/.bashrc_ext
if [ -z "$MACHINE_NAME" ]; then
  echo "Name of this machine: "
  read machine_name
  echo "# Added by setup_my_environment.sh script (Daniel)" >> ~/.bashrc
  echo "export MACHINE_NAME=$machine_name" >> ~/.bashrc
  echo "source ~/.bashrc_ext" >> ~/.bashrc
fi

echo "------- SETTING UP I3 / COMPTON DOTFILES ------------"
mkdir -p ~/.config/i3
ln -s ~/Code/Vitrified-Code/Ubuntu/i3_config ~/.config/i3/config
ln -s ~/Code/Vitrified-Code/Ubuntu/compton_config ~/.config/compton.conf
sudo ln -s ~/Code/Vitrified-Code/Ubuntu/show_a_window_and_do_nothing /usr/bin/

echo "------- SETTING UP GIT ------------"
sudo add-apt-repository ppa:git-core/ppa # to get git 2.9
sudo apt update
sudo apt install -y git
# git config
git config --global user.email "exodaniel@gmail.com"
git config --global push.default simple
git config --global core.hooksPath ~/Code/Vitrified-Code/Ubuntu/git-hooks

echo "------- CLONING PYNIEL ------------"
# git clone git@github.com:danieldugas/Vitrified-Code.git
git clone https://github.com/danieldugas/pyniel.git ~/Code/pyniel

echo "------- SETTING UP GIT-WATCH ------------"
sudo apt install -y python-git
git clone https://github.com/danieldugas/git-watch.git ~/Code/git-watch
# echo "~/Code/Vitrified-Code" >> ~/git_repos_to_watch.txt
# echo "~/Code/pyniel" >> ~/git_repos_to_watch.txt
ln -s ~/Code/Vitrified-Code/Ubuntu/git_repos_to_watch.txt ~/git_repos_to_watch.txt

echo "------- SETTING UP VIM ------------"
sudo apt install -y vim-gtk3
ln -s ~/Code/Vitrified-Code/Vim/vimrc ~/.vimrc
ln -s ~/Code/Vitrified-Code/Ubuntu/flake8_config ~/.config/flake8
mkdir -p ~/.vim/colors
cp ~/Code/Vitrified-Code/Vim/twilight.vim ~/.vim/colors/
git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim

echo "------- SETTING UP HSTR ------------"
# better ctrl-r search
sudo add-apt-repository ppa:ultradvorka/ppa -y
sudo apt-get update
sudo apt-get install hstr -y

echo "------- SETTING UP UMEDIT ------------"
ln -s ~/Code/Vitrified-Code/Ubuntu/man ~/man
ln -s ~/Code/Vitrified-Code/Ubuntu/templates ~/templates

echo "------- SETTING UP LOGBOOK ------------"
git clone git@bitbucket.org:exodaniel/logbook.git ~/logbook

echo "------- SETTING UP UBUNTU GUI PREFERENCES ------------"
## Ubuntu GUI
# Load gnome terminal profile (replace load with dump and < with > to save)
dconf load /org/gnome/terminal/ < ~/Code/Vitrified-Code/Ubuntu/gnome-terminal-profiles.txt
# Gnome Extensions
# PutWindows
{
  cd ~/Downloads && \
  wget https://github.com/negesti/gnome-shell-extensions-negesti/releases/download/v26/putWindow@clemens.lab21.org.zip && \
  file-roller --extract-here putWindow@clemens.lab21.org.zip
  mkdir -p ~/.local/share/gnome-shell/extensions
  mv putWindow@clemens.lab21.org ~/.local/share/gnome-shell/extensions/ && \
  gnome-shell-extension-tool -e putWindow@clemens.lab21.org
}
# Oneclick extract
sudo apt install -y p7zip-full
sudo cp ~/Code/Vitrified-Code/Ubuntu/oneclickextract.desktop /usr/share/applications/
# Load Gnome settings
dconf load / < ~/Code/Vitrified-Code/Ubuntu/dconf-settings.ini
# dconf dump / > ~/Code/Vitrified-Code/Ubuntu/dconf-settings.ini

# compiz config settings manager # DEPRECATED 16.04
# sudo apt install -y compizconfig-settings-manager python-compizconfig
# python ~/Code/Vitrified-Code/Ubuntu/import_compiz_settings.py ~/Code/Vitrified-Code/Ubuntu/compiz.profile
# Enable workspaces
# gsettings set org.compiz.core:/org/compiz/profiles/unity/plugins/core/ hsize 2
# gsettings set org.compiz.core:/org/compiz/profiles/unity/plugins/core/ vsize 2


echo "------- SETTING UP I3 GUI PREFERENCES ------------"
sudo apt install python-pip
pip install --user i3ipc pynput
git clone git@github.com:danieldugas/i3focus.git ~/Code/i3focus
ln -s ~/Code/i3focus/i3focus ~/.i3/i3focus
