
Get-ChildItem -path .\ -Include 'Debug' -Recurse -force | Remove-Item -force -Recurse
Get-ChildItem -path .\ -Include 'Release' -Recurse -force | Remove-Item -force -Recurse
Get-ChildItem -path .\ -Include 'ipch' -Recurse -force | Remove-Item -force -Recurse
Get-ChildItem -path .\ -Include '.vs' -Recurse -force | Remove-Item -force -Recurse
Get-ChildItem -path .\ -Include '*.db' -Recurse -force | Remove-Item -force -Recurse
Get-ChildItem -path .\ -Include '*.aps' -Recurse -force | Remove-Item -force -Recurse
Get-ChildItem -path .\ -Include '*.sdf' -Recurse -force | Remove-Item -force -Recurse
Get-ChildItem -path .\ -Include 'Debug' -Recurse -force | Remove-Item -force -Recurse
Remove-Item hexml_10d.dll
Remove-Item lib\*
Remove-Item xml\lib\*
Remove-Item .\xml\tinyxml\tiny_libd -force -Recurse
Remove-Item .\xml\hexml\Debug_dll -force -Recurse

# powershell -noexeit -executionpolicy bypass -File ./clean.ps1
