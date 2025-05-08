from setuptools import setup, find_packages

setup(
    name="digital_floats_control",
    version="0.1.0",
    packages=find_packages(),
    install_requires=[
        "pyserial>=3.5",
        "requests>=2.31.0",
        "Pillow>=10.0.0",
    ],
    python_requires=">=3.6",
    author="Your Name",
    author_email="your.email@example.com",
    description="Digital Floats Control Application",
    keywords="serial, control, gui",
) 