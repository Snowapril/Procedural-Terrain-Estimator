import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import numpy as np
from PIL import Image
import sys

def main(image_path) :
    full_channel_image = np.array(Image.open(image_path))

    red_channel   = full_channel_image[:,:,0]
    blue_channel  = full_channel_image[:,:,1]
    green_channel = full_channel_image[:,:,2]
    alpha_channel = full_channel_image[:,:,3]
    rgb_channel = full_channel_image[:,:,0:3]

    fig = plt.figure(figsize=(16, 8))
    outer = gridspec.GridSpec(1, 2, wspace=0.2, hspace=0.2)

    inner1 = gridspec.GridSpecFromSubplotSpec(2, 2,
                        subplot_spec=outer[0], wspace=0.2, hspace=0.2)
    colors = [red_channel, blue_channel, green_channel, alpha_channel]
    titles = ["Rock", "Dirt", "Mud", "WetDirt"]

    for j in range(4):
            ax = plt.Subplot(fig, inner1[j])
            ax.imshow(colors[j])
            ax.set_title(titles[j])
            fig.add_subplot(ax)
            
    inner2 = gridspec.GridSpecFromSubplotSpec(1,1,
            subplot_spec=outer[1], wspace=0.1, hspace=0.1)
    ax = plt.Subplot(fig, inner2[0])
    ax.imshow(rgb_channel)
    ax.set_title("RGB Merged")
    fig.add_subplot(ax)

    plt.show()

if __name__ == "__main__" : 
        if len(sys.argv) != 2 :
                print("Usage: ShowBlendMap.py splat_map_path")
        else :
                main(sys.argv[1])
