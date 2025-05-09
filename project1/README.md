### To test this commands

#### To test '<' commands (Input Redirection)

`
sort < input.txt
`


#### To test '>' commands (Output Redirection)
`
ls > files.txt
`


#### To test '>>' commands (Output Append)
`
echo "Hello World" >> greetings.txt
`

#### To test Piping (|)
`
cat input.txt | grep "apple" | sort
`

`
ps aux | grep edge | wc -l
`



#### To test Multiple commands with Semicolon ( ; )
`
pwd; ls; echo "Done listing files"
`

#### To test Multiple commands with Logical AND (&&)
`
mkdir test_folder && cd test_folder && touch file.txt
`

