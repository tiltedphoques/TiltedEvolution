param (
    [string]$path = "..\..\encoding\Structs\Skyrim"
)
$files = Get-ChildItem $path -Filter *_*.cpp 
Set-PSDebug -Trace 0
"// DO NOT EDIT, MACHINE-GENERATED CODE
//
// This is an admittedly dirty approach, but done with the best of intentions.
//
// The idea is to minimize intrusion into the STR Devs's base code, but we 
// need some information that isn't exposed (the behavior variables' strings,
// not just their enum values). This is needed so we can translate the numeric
// original behavior vars back to their string names, so we can then look up the
// NEW numeric value when behaviors are modified.
// 
// That would require a simple code change, but one needed so ubiquitously
// it is intrusive. If the STR team gives permission for the change, this
// can all be cleaned up. Unless and until they do, we make some edited copies
// of the relevant behavior structs in their own namespace to provide the needed info.
//
// Would be so much easier if Nemesis simply added new vars to the end of the 
// list, rather than randomly rearranging it. Rumor is Pandora does it correctly.
//
"


$creatures = "`r`nvoid BehaviorOrigInit()`r`n{`r`n"

foreach ($f in $files)
{
    $path = $f.FullName
    $pathparts = Get-Item $path
    $file = $pathparts.Name
    $creature = $pathparts.BaseName
    $creatures = $creatures + "    (void) $creature::getInstance();`r`n"

    $extract = "`r`nclass $creature`r`n{`r`n  private:"
    $extract 

    # For the enum declaration, match non-greedy to stop at first close brace.
    # Master_Behavior special case since they include more stuff.
    # Also even more special for Fallout 3rd-person, since they inject namespaces
    # we need a look-behind to get rid of it.
    $pattern = "namespace TP"
    $extract = [Regex]::Match((Get-Content $path -Raw), $pattern).Value
    if ([string]::IsNullOrEmpty($extract)) {
        $pattern = "(?s) *enum Variables(.*?)};"
    } else {
        $pattern = "(?s)(?<=namespace TP`r`n{`r`n) *enum Variables(.*?)};"
    }

    $extract = [Regex]::Match((Get-Content $path -Raw), $pattern).Value
    $extract

    $pattern = "key =.*;"
    $extract = [Regex]::Match((Get-Content $path -Raw), $pattern).Value

    # If no key was found in the .cpp file it is because for some creatures it is in .h file
    if ([string]::IsNullOrEmpty($extract))
    {
        $hpath = Get-Item $path
        $hpath = Join-Path $hpath.DirectoryName ($hpath.BaseName + ".h")
        $extract = [Regex]::Match((Get-Content $hpath -Raw), $pattern).Value
    }

    $extract = '    static const uint64_t m_' + $extract + "`r`n"
    $extract

    # Constructor
    $extract = "    // Constructor
    $creature()
    {
        BehaviorOrig::GenerateFromEnum<Variables>(m_key);
    }

    // Singleton
    $creature($creature const&) = delete;
    void operator=($creature const&) = delete;

  public:
    static $creature& getInstance()
    {
        // The only instance. Guaranteed lazy initiated
        // Guaranteed that it will be destroyed correctly
        static $creature instance;

        return instance;
    }`r`n};"

    $extract
}

$creatures = $creatures + "};`r`n"
$creatures
