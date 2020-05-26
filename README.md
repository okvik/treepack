# treepack — file-tree marshaling

## Programs

* treeload directory file
	* Loads a `file` into a tree rooted at `directory` according
	  to rules described later.
* treedump directory file
	* Dumps a tree rooted at `directory` into a `file` in one of the
	  supported formats.

Currently `libucl` is used to read and write `file`s so that `UCL` or
`JSON` files can be loaded with `treeload`; and `JSON` (pretty,
compact), `YAML` (compact), and msgpack, can be dumped with `treedump`.

## Tree-making rules

* Tree leaves, that is non-object, non-array object members or array
	elements become regular files with their key as the file name and
  value as file content.
  Key of array elements is their implicit zero-based index.
* Objects and arrays become directories named after their key. They
  contain files and directories corresponding to child leaves or
  other objects and arrays.

Let's consider a simple example with a structure in `UCL` format:

	people = [
		{name = "Alice", genius = yes, items = [phaser]}
		{name = "Bob", genius = no, items = [cap, stick]},
	]

The resulting file tree produced by `treeload` is as follows:

	people/
		0/
			name    -> Joe
			genius 	-> no
			items/
				0     -> cap
				1     -> stick
		1/
			name    -> Alice
			genius  -> yes
			items/
				0     -> phaser

## TODO

* Write `treedump`.
* Write actual tree-making code instead of emitting a shell script.
* Rewrite as a Lua host (script?) driven by Lua tables.
	* This way any structured format can be generically plugged in by
	  writing a binding and using it to fill a Lua table.
