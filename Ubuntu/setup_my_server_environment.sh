set -x # print commands being executed
echo "------- SETTING UP BASHRC ------------"
# bashrc
ln -s -i ~/Code/Vitrified-Code/Ubuntu/bashrc ~/.bashrc_ext
if [ -z "$MACHINE_NAME" ]; then
  machine_name=$1
  if [ -z $machine_name ]; then
    echo "Name of this machine: "
    read machine_name
  fi
  echo "# Added by setup_my_environment.sh script (Daniel)" >> ~/.bashrc
  echo "export MACHINE_NAME=$machine_name" >> ~/.bashrc
  echo "source ~/.bashrc_ext" >> ~/.bashrc
fi

echo "------- SETTING UP GIT ------------"
# git config
git config --global user.email "exodaniel@gmail.com"
git config --global push.default simple
git config --global core.hooksPath ~/Code/Vitrified-Code/Ubuntu/git-hooks


echo "------- SETTING UP VIM ------------"
ln -s -i ~/Code/Vitrified-Code/Vim/vimrc ~/.vimrc
mkdir -p ~/.config
ln -s -i ~/Code/Vitrified-Code/Ubuntu/flake8_config ~/.flake8
mkdir -p ~/.vim/colors
cp ~/Code/Vitrified-Code/Vim/twilight.vim ~/.vim/colors/
mkdir -p ~/.config/nvim/colors
cp ~/Code/Vitrified-Code/Vim/twilight.vim ~/.config/nvim/colors/
# git clone git@github.com:VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
# vim-plug

echo "------- SETTING UP VSCODE ----------"
mkdir -p ~/.config/Code/User
ln -s ~/Code/Vitrified-Code/Ubuntu/vscode/keybindings.json ~/.config/Code/User/keybindings.json
ln -s ~/Code/Vitrified-Code/Ubuntu/vscode/settings.json ~/.config/Code/User/settings.json

echo "------- SETTING UP HSTR ------------"
