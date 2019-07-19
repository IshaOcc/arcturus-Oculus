## FW Build
Checkout [arcturus-Oculus].

Run these commands to build the FW:

    cd arcturus-sc-master/lib/Drivers/StructureCore/core/nu3000
    ./build/build_target.sh

Output binaries will be located in `./build/bin/target/boot500/boot/`

## Host Build
### MacTester
On a mac, open `arcturus-sc-master/lib/Drivers/StructureCore/apps/mac/StructureCoreDriver/ST02-DevApps.xcodeproj` in Xcode

Edit the MacTester scheme and add this to "Arguments Passed On Launch"

    --bootdir <PATH_TO_FW_BINARIES>

To read/write raw registers from the GUI, toggle "Hard mode" from the _OccActions_ menu

### Linux sample app

    cd arcturus-sc-master/lib/Drivers/StructureCore/core
    ./scripts/buildHost.sh
