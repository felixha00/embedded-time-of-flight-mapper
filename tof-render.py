import numpy as np
import open3d as o3d

if __name__ == "__main__":

    pcd = o3d.io.read_point_cloud("tof_data.txt", format='xyz')
    print(pcd)
    print(np.asarray(pcd.points)) 

    o3d.visualization.draw_geometries([pcd])
    
