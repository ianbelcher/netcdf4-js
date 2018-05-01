{
  "targets": [
    {
      "libraries": [
        "-lnetcdf"
      ],
      "sources": [
        "src/Group.cpp",
        "src/File.cpp",
        "src/Variable.cpp",
        "src/Dimension.cpp",
        "src/Attribute.cpp",
        "src/netcdf4js.cpp"
      ],
      "target_name": "netcdf4",
      "conditions": [
        ['OS=="win"', {
          "include_dirs": [
            "C:\\Program Files\\netCDF 4.6.1\\lib"
          ]
        }]
      ]
    }
  ]
}
