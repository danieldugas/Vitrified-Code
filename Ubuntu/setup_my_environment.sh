# git config
git config --global user.email "exodaniel@gmail.com"

# git clone git@github.com:danieldugas/Vitrified-Code.git
git clone https://github.com/danieldugas/pyniel.git ~/Documents/pyniel



# Vim
sudo apt install vim-gtk3 -y
cp ~/Documents/Vitrified-Code/Vim/vimrc ~/.vimrc
mkdir ~/.vim/colors
cp ~/Documents/Vitrified-Code/Vim/twilight.vim ~/.vim/colors/
git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim

## Ubuntu GUI
# compiz config settings manager
sudo apt install compizconfig-settings-manager python-compizconfig -y
python ~/Documents/Vitrified-Code/Ubuntu/import_compiz_settings.py ~/Documents/Vitrified-Code/Ubuntu/compiz.profile
# Enable workspaces
gsettings set org.compiz.core:/org/compiz/profiles/unity/plugins/core/ hsize 2
gsettings set org.compiz.core:/org/compiz/profiles/unity/plugins/core/ vsize 2
# Oneclick extract
sudo apt install p7zip-full
sudo cp ~/Documents/Vitrified-Code/Ubuntu/oneclickextract.desktop /usr/share/applications/

# bashrc
vimdiff ~/.bashrc ~/Documents/Vitrified-Code/Ubuntu/bashrc

# TODO ctrl+windows+uiojklm,. for moving windows
