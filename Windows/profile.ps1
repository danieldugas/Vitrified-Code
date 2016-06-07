# PowerShell configuration, and aliases
# Copy the contents of this file into your PowerShell profile
# ( Don't have a PowerShell profile?
#  check if you do "Test-Path $Profile"
#  create profile  "New-Item -path $profile -type file –force"
#  find profile    "Get-Variable profile | Format-List" 
#  enable scripts  "Set-ExecutionPolicy Unrestricted -Scope CurrentUser" )
 
function pretty-git-log { git log --graph --abbrev-commit --decorate --date=relative --format=format:'%C(bold blue)%h%C(reset) - %C(bold green)(%ar)%C(reset) %C(white)%s%C(reset) %C(dim white)- %an%C(reset)%C(bold yellow)%d%C(reset)' --all }
Set-Alias gitview  pretty-git-log
function pretty-git-log2 { git log --graph --abbrev-commit --decorate --format=format:'%C(bold blue)%h%C(reset) - %C(bold cyan)%aD%C(reset) %C(bold green)(%ar)%C(reset)%C(bold yellow)%d%C(reset)%n''          %C(white)%s%C(reset) %C(dim white)- %an%C(reset)' --all }
Set-Alias gitview2 pretty-git-log2