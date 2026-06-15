import numpy as np
import pandas as pd
import torch
import torch.nn as nn
import torch.optim as optim
import torchvision.transforms.v2 as T

from torch.utils.data import TensorDataset, DataLoader, random_split, Subset
from torchvision.models import convnext_tiny, ConvNeXt_Tiny_Weights
from torchmetrics.classification import MulticlassAccuracy

from umtools.datasets import load_cats, load_beer, load_beer_test
from umtools.utils import save_image, simulate_png

# %%
# %%
if torch.mps.is_available():
    device = torch.device("mps")
elif torch.cuda.is_available():
    device = torch.device("cuda")
else:
    device = torch.device("cpu")

# %%
import os
import certifi

os.environ["SSL_CERT_FILE"] = certifi.where()
os.environ["REQUESTS_CA_BUNDLE"] = certifi.where()

# %%
# %%
weights = ConvNeXt_Tiny_Weights.DEFAULT
categories = weights.meta["categories"]
preprocess = weights.transforms()        # normalizacja i resize wymagane przez model


model = convnext_tiny(weights=weights).eval().to(device)

for param in model.parameters():
    param.requires_grad_(False)

cats = load_cats()
imgs, labels = cats

img_batch = torch.tensor(imgs, dtype=torch.float32)
img_batch = img_batch.permute(0, 3, 1, 2) / 255.0
img_batch = img_batch.to(device)

target_ids = []

for label in labels:
    target_ids.append(categories.index(label))

target_ids = torch.tensor(target_ids, dtype=torch.long).to(device)

mean = torch.tensor([0.485, 0.456, 0.406], dtype=torch.float32).view(1, 3, 1, 1).to(device)
std = torch.tensor([0.229, 0.224, 0.225], dtype=torch.float32).view(1, 3, 1, 1).to(device)

def normalize_img(x):
    return (x - mean) / std

img_batch.requires_grad_(True)

criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam([img_batch], lr=0.02)

for epoch in range(1, 2001):
    model.eval()

    preds = model(normalize_img(img_batch))
    loss = criterion(preds, target_ids)

    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

    with torch.no_grad():
        img_batch.clamp_(0.0, 1.0)

        png_batch = simulate_png(img_batch.detach().cpu(), normalized=True)
        png_batch = png_batch.to(device)

        png_preds = model(normalize_img(png_batch))
        png_classes = torch.argmax(png_preds, dim=1)

    if epoch % 100 == 0:
        good = (png_classes == target_ids).sum().item()

    if (png_classes == target_ids).all():
        break

save_image(img_batch.detach().cpu(), "not_cat.png", normalized=True)

