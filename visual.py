# # import meshio
# # import vtk

# # mesh = meshio.read('C:/Users/Ксения/palabos/examples/showCases/boundaryCondition/tmp/periodic/Re = 1.5/vtk030000.vti')
# # print(mesh)

# # reader = vtk.vtkXMLImageDataReader()
# # reader.SetFileName("C:/Users/Ксения/palabos/examples/showCases/boundaryCondition/tmp/periodic/Re = 1.5/vtk030000.vti")
# # reader.Update()
# # image = reader.GetOutput()

# # writer = vtk.vtkJPEGWriter()
# # writer.SetInputData(image)
# # writer.SetFileName("C:/Users/Ксения/palabos/examples/showCases/boundaryCondition/tmp/periodic/Re = 1.5/test_vti.jpg")
# # writer.Write()

# import numpy
# from vtk import vtkImageData
# from vtk.util import numpy_support as VN

# filename = "C:/Users/Ксения/palabos/examples/showCases/boundaryCondition/tmp/periodic/Re = 1.5/vtk030000.vtk"

# reader = vtkImageData()
# reader.SetFileName(filename)
# reader.ReadAllVectorsOn()
# reader.ReadAllScalarsOn()
# reader.Update()

# data = reader.GetOutput()

# print(data)

# # dim = data.GetDimensions()
# # vec = list(dim)
# # vec = [i-1 for i in dim]
# # vec.append(3)

# # # print(data)
# # # print(vec)

# # u = VN.vtk_to_numpy(data.GetCellData().GetArray('velocity'))
# # b = VN.vtk_to_numpy(data.GetCellData().GetArray('cell_centered_B'))

# # u = u.reshape(vec,order='F')
# # b = b.reshape(vec,order='F')

# # x = zeros(data.GetNumberOfPoints())
# # y = zeros(data.GetNumberOfPoints())
# # z = zeros(data.GetNumberOfPoints())

# # for i in range(data.GetNumberOfPoints()):
# #         x[i],y[i],z[i] = data.GetPoint(i)

# # x = x.reshape(dim, order='F')
# # y = y.reshape(dim, order='F')
# # z = z.reshape(dim, order='F')











import vtk

filename = "C:/Users/Ксения/palabos/examples/showCases/boundaryCondition/tmp/periodic/Re = 1.5/vtk030000.vti"

# imageData = vtk.vtkImageData()
# imageData.SetDimensions(3, 4, 5)
# if vtk.VTK_MAJOR_VERSION <= 5:
#     imageData.SetNumberOfScalarComponents(1)
#     imageData.SetScalarTypeToDouble()
# else:
#     imageData.AllocateScalars(vtk.VTK_DOUBLE, 1)

# dims = imageData.GetDimensions()
# print(dims)

# Fill every entry of the image data with "2.0"
# for z in range(dims[2]):
#     for y in range(dims[1]):
#         for x in range(dims[0]):
#             imageData.SetScalarComponentFromDouble(x, y, z, 0, 2.0)


# writer = vtk.vtkXMLImageDataWriter()
# writer.SetFileName(filename)
# if vtk.VTK_MAJOR_VERSION <= 5:
#     writer.SetInputConnection(imageData.GetProducerPort())
# else:
#     writer.SetInputData(imageData)
# writer.Write()

# Read the file (to test that it was written correctly)
reader = vtk.vtkXMLImageDataReader()
reader.SetFileName(filename)
reader.Update()

# Convert the image to a polydata
imageDataGeometryFilter = vtk.vtkImageDataGeometryFilter()
imageDataGeometryFilter.SetInputConnection(reader.GetOutputPort())
imageDataGeometryFilter.Update()

data = reader.GetOutput()
print(data)

# mapper = vtk.vtkPolyDataMapper()
# mapper.SetInputConnection(imageDataGeometryFilter.GetOutputPort())

# actor = vtk.vtkActor()
# actor.SetMapper(mapper)
# actor.GetProperty().SetPointSize(3)

# # Setup rendering
# renderer = vtk.vtkRenderer()
# renderer.AddActor(actor)
# renderer.SetBackground(1,1,1)
# renderer.ResetCamera()

# renderWindow = vtk.vtkRenderWindow()
# renderWindow.AddRenderer(renderer)

# renderWindowInteractor = vtk.vtkRenderWindowInteractor()

# renderWindowInteractor.SetRenderWindow(renderWindow)
# renderWindowInteractor.Initialize()
# renderWindowInteractor.Start()