pip install wheel twine pybind11

pandoc --from markdown --to rst zoomg_docs.md -o README.rst # ドキュメント作成

rm -rf zoomg.egg-info/* dist/*
python setup.py sdist
python setup.py bdist_wheel

pyenv local 3.9.1 # twineが3.6.4では動かない
twine upload --repository testpypi dist/* # テスト
# pip --no-cache-dir install --upgrade --index-url https://test.pypi.org/simple/ zoomg # テストダウンロード

# twine upload --repository pypi dist/* # 本番
# pip --no-cache-dir install --upgrade zoomg # 本番ダウンロード
pyenv local 3.6.4