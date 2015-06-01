# Introduction #

Someone else has made a conversion of the World of Dreams scripts to POL 098. This conversion was done with an old version of the scripts, and it's not working 100% either.

What we are doing is converting all of these scripts to POL 099 and update them to work like the 095 scripts.

# Details #

  * Remove all those misleading credits placed in the pkg.cfg files. The author isn't JaceAce, it's either Drocket or Dundee, check the 095 scripts. JaceAce would count as a maintainer at most. But now WE are the maintainers, so he becomes a past contributor, author of the original conversion, but not the scripts.

  * ~~Verify the functionality of each entry in the pkg folder. It should work as in the 095 version. Several scripts are outdated and need deletion, full replacement, and patching to replace deprecated POL functions with the new ones.~~ **Done**

  * Make sure everything is realm aware. Currently it uses `_DEFAULT_REALM`; this should be replaced with the _realm_ property of the character, npc or object in question to permit us to expand to the other realms in the future. There are also some core functions which use a default value when no realm is specified; make sure the realm is always specified.

  * Watch out for removed items. I have found and added some, you may find more.

  * Check objtype numbers. Many of them need to be moved upwards. The range between 0x4000 and 0xFFFF was previously used for multis and custom items, now item graphics were expanded and cover that whole range too. Recommendation is to add 0x10000 to the old objtype number and add an OldObjType property with the old number to each modified item entry. The OldObjType property will be necesary once for the migration process, and will be removed after that, so we will need both files with and without them, as some new item objtypes will overlap with the OldObjType numbers. As for the boats and houses, please leave them the way they are, I will take care of that myself, it's delicate and tricky stuff (Agata).