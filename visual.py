import meshio
import vtk

# mesh = meshio.read('C:/Users/Ксения/palabos/examples/showCases/boundaryCondition/tmp/periodic/Re = 1.5/vtk030000.vti')
# print(mesh)

reader = vtk.vtkXMLImageDataReader()
reader.SetFileName("C:/Users/Ксения/palabos/examples/showCases/boundaryCondition/tmp/periodic/Re = 1.5/vtk030000.vti")
reader.Update()
image = reader.GetOutput()

writer = vtk.vtkJPEGWriter()
writer.SetInputData(image)
writer.SetFileName("test_vti.jpg")
writer.Write()